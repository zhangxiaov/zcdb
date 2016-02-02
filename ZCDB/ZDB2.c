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


//读取 table文件，文件内存recordID，每个ID8 byte，dir ： tables
ZMap* zdbTablesInit() {
    if (dbDirPath == NULL || strlen(dbDirPath) == 0) {
        throw("dbDir is NULL");
    }
    
    ZArray* names = zfileNamesByPath(csAppend(dbDirPath, tableDirName));
    tablesMap = zmapInit();
    int count = names->len;
    for (int i = 0; i < count; i++) {
        char* table_name = zarrayGet(names, i);
        zmapPut(tablesMap, table_name, table_name);
    }
    
    return tablesMap;
}