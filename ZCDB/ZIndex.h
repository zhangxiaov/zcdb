//
//  ZIndex.h
//  ZC
//
//  Created by zhangxinwei on 16/1/28.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#ifndef ZIndex_h
#define ZIndex_h

#include <stdio.h>
#include "ZArray.h"

//字段之索引，便于根据字段 对记录分组,
//索引文件名：index_xxxx，格式：
//8byte,记录id1，记录id2，，，
//整型8byte，字符 11byte，大小随值,值1，值2，，，，

typedef struct _ZIndex ZIndex;

struct _ZIndex {
    int type;
    int typeSize;
    char* name;
    int valueTypeSpace;
    int valueType; //手动设置，或读取文件名设置
    int ridCount;
    char* indexIDFileName;
    char* indexValFileName;
};

//若不在内存，从目录下读
ZArray* zindexArray(ZArray* indexArray);

//创建一个索引文件 self == indexArray，手动创建时调用
void zindexCreate(ZArray* indexArray, char* indexName, int valueType);

//追加，当新增一条记录含此字段
void zindexAppend(ZIndex* self, unsigned long intval, char* strval, unsigned long pos, int valType);

//改，当修改纪录此字段值
void zindexUpdate(ZIndex* self, unsigned long intval, char* strval, unsigned long pos, int valType);

//销毁文件
void zindexDestory(ZIndex* self);

#endif /* ZIndex_h */
