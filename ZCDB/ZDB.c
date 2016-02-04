//
//  ZDB.c
//  ZCDB
//
//  Created by 张新伟 on 16/2/1.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#include "ZDB.h"
#include "ZJson.h"
#include "ZMap.h"
#include "ZFile.h"
#include "CString.h"
#include "ZSearch.h"
#include "ZSort.h"

// value = fileData
// 索引 字段 <＝ 11byte
static ZMap* tablesMap = NULL;
static char* dbDirPath = NULL;
static const char* tableDirName = "table";
static const char* indexIDDirName = "indexID";
static const char* indexValDirName = "indexVal";
static const char* indexTypeText = "Text";
static const char* indexTypeNumber = "Number";

struct zindex {
    ulong rcount;
    ulong* ridData;
    char* valData;
    char* indexName;
    char* indexType;
    struct zindex* next;
};

struct zdbIndexInfo {
    ulong indexCount;
    struct zindex* indexs;
};

struct zdbFile {
    void* ptr;
    char* fileName;
    int fileSize;
    int recordSize;
    int recordStartID; // first 1
    int recordEndID;//rc = fs/rs
};

struct zdb {
    int fileCount;
    ulong globalID; //atomic
    struct zdbFile* files;
};

struct zdbtable {
    ulong size;
    ulong* data;
    struct zdbIndexInfo indexInfo;
};

struct zdbrecyclebin {
    ulong rid;
    char* data;
};



static struct zdb db;

void zdbatomic();
void zdbTableDataAppend(char* tableName, ulong rid);


// {"table":"t_user", "name":"zhangxinwei", "age":26, "class":{"class_id":"11", "class_name":"xxxxxx"}}
ulong zdbInsert(char* jsonStr) {
    ZJson* json = zjsonInitWithString(jsonStr);
    ZMap* map = json->data;
    
    //含table否
    char* table_name = zmapGet(map, "table");
    if (table_name == NULL) {
        throw("record have no table_name field");
    }
    if (tablesMap == NULL) {
        throw("tablesMap need init");
    }
    
    //save jsonstr to mmap
    zdbatomic();
    size_t size = sizeof(struct zdbFile);
    size_t jLen = strlen(jsonStr);
    for (int i = 0; i < db.fileCount; i++) {
        struct zdbFile* f = (struct zdbFile*)(db.files + i*size);
        if (f->recordEndID > db.globalID+1) {
            ulong offset = (db.globalID +1)*f->recordSize;
            if (jLen > f->recordSize) {
                //截断
                throw("");
            }
            memcpy(f->ptr+offset, jsonStr, jLen);
            ++db.globalID;
            zdbTableDataAppend(table_name, db.globalID);
            break;
        }
    }
    
    //索引
    struct zdbtable* t = (struct zdbtable*)zmapGet(tablesMap, table_name);
    struct zdbIndexInfo info = t->indexInfo;
    int indexCount = (int)info.indexCount;
    for (int i = 0; i < indexCount; i++) {
        char* indexName = info.indexs->indexName;
        char* val = zmapGet(map, indexName);
        if (val != NULL) {
            zdbIndexDataAppend(table_name, indexName, db.globalID, val);
        }
    }
    
    return db.globalID;
}


//
char* zdbSelectOne(ulong rid) {
    char* val = NULL;
    if (tablesMap == NULL) {
        throw("tablesMap need init");
    }
    
    size_t size = sizeof(struct zdbFile);
    for (int i = 0; i < db.fileCount; i++) {
        struct zdbFile* f = (struct zdbFile*)(db.files + i*size);
        if (rid < f->recordEndID) {
            ulong offset = rid*f->recordSize;
            val = f->ptr+offset;
            break;
        }
    }
    
    return val;
}

void zdbatomic() {
//    for (int i = 0; i < db.fileCount; i++) {
//        struct zdbFile* f = (struct zdbFile*)(db.files + i*(sizeof(struct zdbFile)));
//        if (f->recordEndID > db.globalID+1) {
//            ulong offset = (db.globalID +1)*f->recordSize;
//            
//            memcpy(<#void *#>, <#const void *#>, <#size_t#>)
//        }
//    }
}


//table data 有序
//========================================================
//读取 table文件，文件内存recordID，每个ID8 byte，dir ： tables
// 此用于重启读取table文件名
ZMap* zdbTableInit() {
    if (dbDirPath == NULL || strlen(dbDirPath) == 0) {
        throw("dbDir is NULL");
    }
    
    char* tableDirPath = csPathAppendComponent(dbDirPath, tableDirName);
    ZArray* names = zfileNamesByPath(tableDirPath);
    tablesMap = zmapInit();
    int count = names->len;
    for (int i = 0; i < count; i++) {
        char* table_name = zarrayGet(names, i);
        struct zdbtable t;
        t.data = (ulong*)zfileReadAllData(csPathAppendComponent(tableDirPath, table_name), &t.size);
        t.size = t.size/8;
        zmapPut(tablesMap, table_name, &t);
    }
    
    return tablesMap;
}

//通过后台建新表
ZMap* zdbTableCreate(char* tableName) {
    if (dbDirPath == NULL || strlen(dbDirPath) == 0) {
        throw("dbDir is NULL");
    }
    
    if (tablesMap == NULL) {
        tablesMap = zmapInit();
    }
    
    zmapPut(tablesMap, tableName, NULL);
    return tablesMap;
}


//get count data
//ulong* zdbTableDataGet(char* tableName, int start, int count) {
//    struct zdbtable* t = (struct zdbtable*)zmapGet(tablesMap, tableName);
//    
//}

// when insert new record
void zdbTableDataAppend(char* tableName, ulong rid) {
    struct zdbtable* t = (struct zdbtable*)zmapGet(tablesMap, tableName);
    ulong* oldData = t->data;
    ulong* newData = (ulong*)malloc((t->size+1)*8);
    memcpy(newData, oldData, t->size*8);
    memcpy(newData+t->size*8, &rid, 8);
    ++t->size;
    
    free(oldData);
}

// when del record
void zdbTableDataDel(char* tableName, ulong rid) {
    //二分查找 rid
    struct zdbtable* t = (struct zdbtable*)zmapGet(tablesMap, tableName);
    int index = zbinarySearchForUlong(t->data, rid, 0, (int)t->size-1);
    if (index < 0) {
        return;
    }
    
    ulong* oldData = t->data;
    ulong* newData = (ulong*)malloc((t->size-1)*8);
    memcpy(newData, oldData, index*8);
    memcpy(newData+index*8, oldData+(index+1)*8, (t->size-index-1)*8);
    --t->size;
    
    free(oldData);
}

// when record from recycle bin return
void zdbTableDataInsert(char* tableName, struct zdbrecyclebin rb) {
    struct zdbtable* t = (struct zdbtable*)zmapGet(tablesMap, tableName);
    
    int index = zbinarySearchForOutterUnlong(t->data, rb.rid, 0, (int)t->size - 1);
    
    ulong* oldData = t->data;
    ulong* newData = (ulong*)malloc((t->size+1)*8);
    memcpy(newData, oldData, index*8);
    memcpy(newData+index+index*8, oldData+index*8, (t->size-index)*8);
    ++t->size;
    
    free(oldData);
}

//write to hd
void zdbTableDataflush(char* tableName) {
    
}

//index
//=====================================
//此重启时用
void zdbIndexInit(char* tableName) {
    if (dbDirPath == NULL || strlen(dbDirPath) == 0) {
        throw("dbDir is NULL");
    }
    
    char* iddirpath = csPathAppendComponent(csPathAppendComponent(dbDirPath, tableName), indexIDDirName);
    char* valdirpath = csPathAppendComponent(csPathAppendComponent(dbDirPath, tableName), indexValDirName);
    struct zdbtable* t =  (struct zdbtable*)zmapGet(tablesMap, tableName);

    //ID
    ZArray* names = zfileNamesByPath(iddirpath);
    int count = names->len;
    
    struct zdbIndexInfo indexInfo;
    indexInfo.indexCount = count;
    indexInfo.indexs = NULL;
    
    t->indexInfo = indexInfo;
    
    for (int i = 0; i < count; i++) {
        struct zindex* index = indexInfo.indexs;
        if (index == NULL) {
            index = (struct zindex*)malloc(sizeof(struct zindex));
            char* indexidFileName = zarrayGet(names, i);
            index->indexName = indexidFileName;
            index->ridData = (ulong*)zfileReadAllData(csPathAppendComponent(iddirpath, indexidFileName), &index->rcount);
            index->valData = zfileReadAllData(csPathAppendComponent(valdirpath, indexValDirName), 0);
            index->rcount = index->rcount/8;
            index->next = NULL;
        }else {
            struct zindex* indexMid = (struct zindex*)malloc(sizeof(struct zindex));
            indexMid->next = index;
            char* indexidFileName = zarrayGet(names, i);
            indexMid->indexName = indexidFileName;
            indexMid->ridData = (ulong*)zfileReadAllData(csPathAppendComponent(iddirpath, indexidFileName), &indexMid->rcount);
            indexMid->valData = zfileReadAllData(csPathAppendComponent(valdirpath, indexValDirName), 0);
            indexMid->rcount = indexMid->rcount/8;
            
            indexInfo.indexs = indexMid;
        }
    }
}

//后台
// db/table/indexID
bool zdbIndexCreate(char* tableName, char* indexName, char* indexType) {
    if (indexType == NULL) {
        return false;
    }
    
    struct zdbtable* t =  (struct zdbtable*)zmapGet(tablesMap, tableName);
    struct zdbIndexInfo info = t->indexInfo;
    struct zindex* p = (struct zindex*)malloc(sizeof(struct zindex));
    
    char* tableDirPath = csPathAppendComponent(dbDirPath, tableDirName);
    char* id_filePath = csPathAppendComponent(tableDirPath, indexIDDirName);
    char* val_filePath = csPathAppendComponent(tableDirPath, indexValDirName);
    
    //1 create index file
    if(!zfileCreateFileWithName(id_filePath)) {
        return false;
    }
    if (!zfileCreateFileWithName(val_filePath)) {
        return false;
    }
    
    
    //2 fill index file from table rid
    //此步耗时
    ulong* ridData = t->data;
    int size = (int)t->size;
    char* data = (char*)malloc(12*size);
    memset(data, 0, 12*size);
    
    for (int i = 0; i < size; i++) {
        ulong rid = *(ridData+i*8);
        char* val = csSearchValByCVCForJson(zdbSelectOne(rid), indexName, 11);
        memcpy(data+i*12, val, 11);
    }
    
    if (csIsEqual(indexType, indexTypeNumber)) {
        //按 数字 排序
        zsortForStringPairAsNumber(ridData, data, 0, size-1);
    }else if (csIsEqual(indexType, indexTypeText)) {
        //值 按 字符串排序
        zsortForStringPairAsString(ridData, data, 0, size-1);
    }
    
    p->indexName = indexName;
    p->indexType = indexType;
    p->ridData = ridData;
    p->valData = data;
    p->rcount = size;
    p->next = info.indexs;
    info.indexs = p;

    //end
    
    return false;
}

//索引追加 皆排序 when insert
void zdbIndexDataAppend(char* tableName, char* indexName, ulong rid, char* val) {
    struct zdbtable* t =  (struct zdbtable*)zmapGet(tablesMap, tableName);
    struct zdbIndexInfo info = t->indexInfo;
    
    struct zindex* p;
    p = info.indexs;
    while (p) {
        if (csIsEqual(p->indexName, indexName)) {
            int count = (int)p->rcount;
            int index = zbinarySearchNearPosForString(p->valData, val, 0, count, 11);
            
            ulong* oldIDData = p->ridData;
            char* oldValData = p->valData;
            ulong* newIDData = (ulong*)malloc((p->rcount+1)*8);
            char* newValData = (char*)malloc((p->rcount+1)*11);
            
            memcpy(newIDData, oldIDData, index*8);
            memcpy(newIDData+index+index*8, oldIDData+index*8, (p->rcount-index)*8);
            
            memcpy(newValData, oldValData, index*11);
            memcpy(newValData+index+index*11, oldValData+index*11, (p->rcount-index)*11);
            
            free(oldIDData);
            free(oldValData);
            
            ++p->rcount;
            break;
        }
        
        p = p->next;
    }
}












