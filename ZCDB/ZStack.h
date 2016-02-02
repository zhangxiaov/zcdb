//
//  ZStack.h
//  ZC
//
//  Created by 张新伟 on 16/1/2.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#ifndef ZStack_h
#define ZStack_h

#include <stdio.h>
#include "ZC.h"
#include "ZMap.h"
#define maxsize 10




typedef struct _ZStack ZStack;

struct _ZStack {
    void* data;
    int top;
};

ZStack* zstackInit();

void zstackPushByte(ZStack* self, char c);
char zstackPopByte(ZStack* self);

void zstackPush(ZStack* self, void* valPtr);
void* zstackPop(ZStack* self);
void* zstackPeek(ZStack* self);
bool zstackEmpty(ZStack* self);
int zstackPreValueType(ZStack* self);
#endif /* ZStack_h */
