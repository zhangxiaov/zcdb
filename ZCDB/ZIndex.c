//
//  ZIndex.c
//  ZC
//
//  Created by zhangxinwei on 16/1/28.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#include "ZIndex.h"
#include "CString.h"
#include "ZFile.h"

//字段之索引，便于根据字段 对记录分组,
//索引文件名：index_xxxx，格式：
//8byte,记录id1，记录id2，，，
//整型8byte，字符 11byte，大小随值,值1，值2，，，，

static int index_idSpace;
static int intvalueSpace = 8;
static int strvalueSpace = 11;


ZIndex* zindexInit() {
    ZIndex* index = (ZIndex*)malloc(sizeof(ZIndex));
    return index;
}

ZIndex* zindexInitWithValues(char* name, int valueTypeSpace) {
    ZIndex* index = (ZIndex*)malloc(sizeof(ZIndex));
    index->name = name;
    index->valueTypeSpace = valueTypeSpace;
    
    return index;
}

//创建一个索引文件 self == indexArray，手动创建时调用
void zindexCreate(ZArray* indexArray, char* indexName, int valueType) {
    indexArray = zindexArray(indexArray); //判空
    
    //1 新索引加入 数组中
    ZIndex* index = zindexInit();
    index->name = indexName;
    index->valueTypeSpace = valueType;
    zarrayAdd(indexArray, index);
    
    //2 创建索引 文件 indexid_xxx indexint_xxx indexstr_xxx
    char* fileName = csAppend("indexid_", indexName);
    zfileCreate(fileName);
    index->indexIDFileName = fileName;
    
    char* temp = NULL;
    if (valueType == typeInt) {
        temp = "indexint_";
    }else if (valueType == typeString) {
        temp = "indexstr_";
    }else {
        assert(valueType <= 0);
    }
    char* fileName2 = csAppend(temp, indexName);
    zfileCreate(fileName2);
    index->indexValFileName = fileName2;
}


//追加，当新增一条记录含此字段
void zindexAppend(ZIndex* self, unsigned long intval, char* strval, unsigned long pos, int valType) {
    //忽略 多线程写文件 每次写8个字节 于id文件
    zfileWriteInt(self->indexIDFileName, pos);
    
    if (valType == typeString) {
        int len = (int)strlen(strval);
        assert(len <= strvalueSpace);
        zfileWriteStringSpecifySize(self->indexValFileName, strval, strvalueSpace);
    }else if (valType == typeInt){
        zfileWriteULongSpecifySize(self->indexValFileName, intval, intvalueSpace);
    }
}

//改，当修改纪录此字段值
void zindexUpdate(ZIndex* self, unsigned long intval, char* strval, unsigned long pos, int valType) {
    //忽略 多线程写文件 每次写8个字节 于id文件
    int the = (int)pos/recordSize;
    
    //只改值
    unsigned long valpos = 0;
    if (valType == strvalueSpace) {
        valpos = the*strvalueSpace;
        zfileWriteStringSpecifySizeWithPos(self->indexValFileName, strval, strvalueSpace, valpos);
    }else if (valType == intvalueSpace) {
        valpos = the*intvalueSpace;
        zfileWriteULongSpecifySizeWithPos(self->indexValFileName, intval, intvalueSpace, valpos);
    }
}

//销毁文件
void zindexDestory(ZIndex* self) {
    zfileDestory(self->indexIDFileName);
    zfileDestory(self->indexValFileName);
    free(self);
}

////得值的类型
//int zindexGetValueTypeSize(char* indexName) {
//    if (zfileIsExist(csAppend("indexint_", indexName), "/tmp/db/")) {
//        return typeInt;
//    }
//    
//    if (zfileIsExist(csAppend("indexstr_", indexName), "/tmp/db/")) {
//        return typeString;
//    }
//    
//    return 0;
//}

//若不在内存，从目录下读
ZArray* zindexArray(ZArray* indexArray) {
    if (indexArray != NULL) {
        return indexArray;
    }
    
    ZArray* array = zarrayInit(true);
    
    ZArray* fileNames = zarrayFileNameWithPrefix("/tmp/db/", "indexid");
    int count = fileNames->len;
    for (int i = 0; i < count; i++) {
        char* fileName = zarrayGet(fileNames, i);
        char* n = csSeparateToEnd(fileName, "_");
        ZIndex* index = zindexInit();
        index->name = n;
        
        if (zfileIsExist(csAppend("indexint_", fileName), "/tmp/db/")) {
            index->valueType = typeInt;
            index->valueTypeSpace = intvalueSpace;
        }else if (zfileIsExist(csAppend("indexstr_", fileName), "/tmp/db/")) {
            index->valueType = typeString;
            index->valueTypeSpace = strvalueSpace;
        }
        
        zarrayAdd(array, index);
    }
    
    return array;
}

