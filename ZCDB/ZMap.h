//
//  ZMap.h
//  oc_demo
//
//  Created by 张新伟 on 15/12/12.
//  Copyright (c) 2015年 zhangxinwei. All rights reserved.
//

#ifndef __oc_demo__ZMap__
#define __oc_demo__ZMap__

#include <stdio.h>
#include "ZArray.h"

typedef struct _zmapNode zmapNode;

struct _zmapNode {
    char* key;
    void* val;
    zmapNode* next;
};


typedef struct _ZMap ZMap;
struct _ZMap {
    int type;
    int typeSize;
    zmapNode* data;
    int size;
    int count;
};



#define ZMAP 11

ZMap* zmapInit();
ZMap* zmapInitWithSize();
void zmapPut(ZMap* self, char* key, void* val);
void* zmapGet(ZMap* self, char* key);
char* zmapToString(ZMap* self);

#endif /* defined(__oc_demo__ZMap__) */
