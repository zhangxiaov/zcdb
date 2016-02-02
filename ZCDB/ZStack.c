//
//  ZStack.c
//  ZC
//
//  Created by 张新伟 on 16/1/2.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#include "ZStack.h"
#include <malloc/malloc.h>
#include <string.h>

ZStack* zstackInit(int size, int type) {
    ZStack* s = (ZStack*)malloc(sizeof(ZStack));
    
    switch (type) {
        case typeByte:
            s->data = (char*)malloc(sizeof(char)*size);
            break;
        case typePtr:
            s->data = (void*)malloc(8*size);
        default:
            break;
    }
    if (!s) {
        printf("空间不足");
    }
    
    s->top = -1;
    
    return s;
}

void zstackPush(ZStack* self, void* valPtr) {
    if (self->top == maxsize - 1) {
        printf("over\n");
        return;
    }
    
    self->top ++;
    unsigned long* data = self->data;
    data += self->top;
    
    unsigned long p = valPtr;
    *data = p;
}

void* zstackPop(ZStack* self) {
    if (self->top == -1) {
        printf("empty\n");
        return NULL;
    }
    
    self->top--;
    unsigned long* data = self->data;
    data += (self->top+1);
    
    unsigned long valPtr = *data;
    
    return valPtr;
}

int zstackPreValueType(ZStack* self) {
    ZStruct* data = zstackPeek(self);
    int type = data->type;
    return type;
}

bool zstackEmpty(ZStack* self) {
    if (self->top == -1) {
        return true;
    }
    
    return false;
}

void* zstackPeek(ZStack* self) {
    if (self->top == -1) {
        printf("empty\n");
        return NULL;
    }
    
//    self->top--;
    unsigned long* data = self->data;
//    data += (self->top+1);
    data += self->top;
    
    unsigned long valPtr = *data;
    
    return valPtr;

}

void zstackPushByte(ZStack* self, char c) {
    if (self->top == maxsize - 1) {
        printf("over\n");
        return;
    }
    
    self->top ++;
    char* data = self->data;
    data[self->top] = c;
}

char zstackPopByte(ZStack* self) {
    if (self->top == -1) {
        printf("empty\n");
        return 0;
    }
    
    self->top--;
    char* data = self->data;

    return data[self->top+1];
}

typedef struct _testNode testNode;

struct _testNode {
    char* key;
    char* val;
};

testNode* testInit(char* k, char* v) {
    testNode* t = (testNode*)malloc(sizeof(testNode));
    t->key = k;
    t->val = v;
    
    return t;
}

void main_stack() {
//void main() {
    
    //base type
//    char s[10] = {'a', 'b', 'c', 'd', 'e','f', 'g', 'h', 'i', 'j'};
//    
//    ZStack* stack = zstackInit(10, typeByte);
//    
//    for (int i = 0; i < 12; i++) {
//        zstackPushByte(stack, s[i]);
//    }
//    
//    for (int i = 0; i < 12; i++) {
//        printf("%c \n", zstackPopByte(stack));
//    }
    
    //ptr type
    char* key[10] = {"aa", "bb", "cc", "dd", "ee", "ff", "gg", "hh", "ii", "jj"};
    char* val[10] = {"aav", "bbv", "ccv", "ddv", "eev", "ffv", "ggv", "hhv", "iiv", "jjv"};

    
    ZStack* stack2 = zstackInit(10, typePtr);

    for (int i = 0; i < 12; i++) {
        testNode* t1 =  testInit(key[i], val[i]);
        zstackPush(stack2, t1);
    }
    
    for (int i = 0; i < 12; i++) {
        testNode* t = (testNode*)zstackPop(stack2);
        if (t == NULL) {
            break;
        }
        printf("key = %s, val = %s \n", t->key, t->val);
    }
}