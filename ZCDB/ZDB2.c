//
//  ZDB2.c
//  ZCDB
//
//  Created by 张新伟 on 16/2/1.
//  Copyright © 2016年 张新伟. All rights reserved.
//

// db --file1,file2,file3
//      table--t_user,t_class
//      index-- i_age
#include "ZDB2.h"
#include "ZJson.h"
#include "ZMap.h"
#include "ZFile.h"
#include "CString.h"
#include "ZSearch.h"

// value = fileData
static ZMap* tablesMap = NULL;
static char* dbDirPath = NULL;
static const char* tableDirName = "table";

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
};

struct zdbrecyclebin {
    ulong rid;
    char* data;
};

static struct zdb db;

void zdbatomic();


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
            break;
        }
    }
    
    //索引
    
    
    return 0;
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
    memcpy(newData+index+index*8, oldData, size_t);
    
}

//write to hd
void zdbTableDataflush(char* tableName) {
    
}

//index
//=====================================
//此重启时用













