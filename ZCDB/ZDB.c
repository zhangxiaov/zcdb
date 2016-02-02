//
//  ZDB.c
//  ZC
//
//  Created by 张新伟 on 16/1/12.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#include "ZDB.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "ZMap.h"
#include <string.h>
#include <sys/malloc.h>
#include "ZTime.h"
#include <assert.h>
#include "CString.h"

static int globalPos = 0;

//db程式，无table，只record

void* zfileToMmap(char* fileName) {
    int fd = open(fileName, O_RDWR);
    assert(fd > 0);
    struct stat st;
    fstat(fd, &st);
    void* addr = mmap(0, st.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    
    printf("%llu\n", addr);
    printf("%llu\n", addr+1);

    printf("%llu\n", addr+st.st_size);
    return addr;
}

unsigned long zdbAddByString(zdb* self, char* val) {
    int s = recordSize;
    if (strlen(val) > s) {
        printf("over recordSize\n");
        return 0;
    }
    
    memcpy(self->mmap+globalPos, val, strlen(val));
    globalPos += recordSize;
    
    
    //处理索引
    //
    
    return globalPos-recordSize;
}

//插一跳新纪录
unsigned long zdbInsertByJson(zdb* self, ZJson* json) {
    unsigned long pos = 0;
    char* jsonStr = zjsonToString(json);
    size_t jsonStrLen = strlen(jsonStr);
    
    int size = recordSize;
    if ( jsonStrLen > size) {
        printf("over recordSize\n");
        return 0;
    }
    
    memcpy(self->mmap+globalPos, jsonStr, jsonStrLen);
    globalPos += recordSize;
    pos = globalPos-recordSize;

    //处理索引，json中一层key 在indexarray 中，即行zindexadd
    ZMap* map = json->data;
    ZArray* indexArray = self->indexArray;
    
    int count = 0;
    if (indexArray != NULL) {
        count = indexArray->len;
    }
    for (int i = 0; i < count; i++) {
        ZIndex* index = zarrayGet(indexArray, i);
        switch (index->valueType) {
            case typeInt:{
                char* val = zmapGet(map, index->name);
                zindexAppend(index, csToInt(val), NULL, pos, typeInt);
            }
                break;
            case typeString:{
                char* val = zmapGet(map, index->name);
                zindexAppend(index, 0, val, pos, typeString);
            }
                break;
            default:
                break;
        }
    }

    return pos;
}

void zdbUpdateByJson(zdb* self, ZJson* json) {
    
}

void zdbUpdate(zdb* self, int pos, char* val) {
    int s = recordSize;
    if (strlen(val) > s) {
        printf("over recordSize\n");
        return ;
    }
    memcpy(self->mmap+pos, val, strlen(val));
}

ZJson* zdbReadToJson(zdb* self, int pos) {
    int s = recordSize;
    char* jsonStr = (char*)malloc(s);
    memcpy(jsonStr, self->mmap+pos, s);
    ZJson* json = zjsonInitWithString(jsonStr);
    return json;
}

char* zdbReadToString(zdb* self, int pos) {
    int s = recordSize;
    char* jsonStr = (char*)malloc(s);
    memcpy(jsonStr, self->mmap+pos, s);
    return jsonStr;
}

zdb* zdbInit(char* fileName) {
    static zdb* db = NULL;
    if (db == NULL) {
        db = (zdb*)malloc(sizeof(zdb));
        db->indexArray = NULL;
        
        void* p = zfileToMmap(fileName);
        if (p == NULL) {
            return NULL;
        }
        db->mmap = p;
    }
    
    return db;
}

void zdbClose(zdb* self, int size) {
    munmap(self->mmap, size);
}

bool zdbCreate(char* fileName, int size) {
    int fd = open(fileName, O_RDWR);
    if (fd < 0) {
        printf("create new db file failed, desc:%s\n", strerror(errno));
        return false;
    }
    
    int ret = truncate(fileName, size);
    if (ret < 0) {
        printf("truncate new db file failed, desc:%s\n", strerror(errno));
        return false;
    }
    
    close(fd);
    return true;
}

void main_zdb() {

//void main() {


    
    int len = sizeof(zrecord);
    
    char* dbName = "/tmp/test";
    int dbSize = 1000000;
    if (!zdbCreate(dbName, 100000000)) {
        return;
    }
    
    zdb* db = zdbInit(dbName);
    
    ZMap* map = zmapInit();
    zmapPut(map, "userName", "张新伟");
    zmapPut(map, "password", "123456");
    
    time_t s = ztimeSince1970();
    for (int i = 0; i < 10000; i++) {
        int pos = zdbAdd(db, zmapToString(map));
        zdbReadToString(db, pos);
    }
    
    printf("time = %.0f\n", difftime(ztimeSince1970(), s));
    
    int pos = zdbAdd(db, zmapToString(map));
    printf("pos = %d,\n content = %s\n", pos, zdbReadToString(db, pos));
    
    zmapPut(map, "age", "26");
    zdbUpdate(db, pos, zmapToString(map));
    printf("pos = %d,\n content = %s\n", pos, zdbReadToString(db, pos));
    
    //2
    int pos2 = zdbAdd(db, zmapToString(map));
    printf("pos = %d,\n content = %s\n", pos2, zdbReadToString(db, pos2));
    
    zmapPut(map, "age", "26");
    zdbUpdate(db, pos2, zmapToString(map));
    printf("pos = %d,\n content = %s\n", pos2, zdbReadToString(db, pos2));

    zdbClose(db, dbSize);
}



















