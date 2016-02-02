//
//  ZMap.c
//  oc_demo
//
//  Created by 张新伟 on 15/12/12.
//  Copyright (c) 2015年 zhangxinwei. All rights reserved.
//

#include "ZMap.h"
#include <sys/malloc.h>
#include "ZHash.h"
#include <string.h>
#include "CString.h"
#include <stdlib.h>
#include "ZTime.h"




zmapNode* zmapGetNode(ZMap* self, char* key, int index);
void zmapReHash(ZMap* self, char* akey, void* aval);

ZMap* zmapInit() {
    ZMap* map = (ZMap*)malloc(sizeof(ZMap));
    map->type = typeZMap;
    map->typeSize = sizeof(ZMap);
    map->size = 11;
    
    zmapNode* nodeData = (zmapNode*)malloc(sizeof(zmapNode)*11);
    memset(nodeData, 0, sizeof(zmapNode)*11);
    
    map->data = nodeData;
    return map;
}

ZMap* zmapInitWithSize(int size) {
    ZMap* map = (ZMap*)malloc(sizeof(ZMap)*size);
    map->type = typeZMap;
    map->typeSize = sizeof(ZMap);
    map->size = size;
    
    return map;
}

ZMap* zmapInitWithKV(int n, char* key,...) {
    ZMap* map = (ZMap*)malloc(sizeof(ZMap)*11);
    map->type = typeZMap;
    map->typeSize = sizeof(ZMap);
    map->size = 11;
    
    return map;
}

void zmapPut(ZMap* self, char* key, void* val) {
    if (self == NULL || key == NULL) {
        return;
    }
    
    uint64_t h = zhashCode(key, strlen(key));
    int index = (int)(h % self->size);
    
   
    // check
    ++self->count;
    float f = self->count / self->size;
    if (f >= 0.75) {
        zmapReHash(self, key, val);
        return;
    }

    // < 0.75
    zmapNode* node = self->data + index;
    for (; node != NULL; node = node->next) {
        if (node->key == NULL) {
            node->key = key;
            node->val = val;

            return;
        }
        if (csIsEqual(key, node->key)) {
            node->val = val;

            return;
        }
        if (node->next == NULL) {
            zmapNode* newNode = (zmapNode*)malloc(sizeof(zmapNode));
            newNode->key = key;
            newNode->val = val;
            newNode->next = NULL;
            node->next = newNode;
            
            return;
        }
    }
}

void zmapReHash(ZMap* self, char* akey, void* aval) {
    int oldSize = self->size;
    int oldCount = self->count;
    
    int newSize = oldSize*2 + 1;
    int newCount = oldCount + 1;
    
    self->count = newCount;
    self->size = newSize;
    
    int index = 0;
    int keyIndex = 0;
    
    zmapNode* oldNodeData = self->data;
    
    zmapNode* newNodeData = (zmapNode*)malloc(newSize*sizeof(zmapNode));
    memset(newNodeData, 0, newSize*sizeof(zmapNode));
    
    while (index < oldSize) {
        zmapNode* node = oldNodeData + index;
        for (; node != NULL; node = node->next) { //old node data
            if (node->key == NULL) {
                break;
            }
            char* key = node->key;
           
            keyIndex = zhashCode(key, strlen(key)) % newSize;
            zmapNode* addr = newNodeData + keyIndex;
            
            for (; addr != NULL; addr = addr->next) { //new node data
                if (addr->key == NULL) {
                    addr->key = node->key;
                    addr->val = node->val;

                    goto label1;
                }
                if (csIsEqual(key, addr->key)) {
                    addr->val = node->val;

                    goto label1;
                }
                
                if (addr->next == NULL) {
                    zmapNode* newNode = (zmapNode*)malloc(sizeof(zmapNode));
                    newNode->key = node->key;
                    newNode->val = node->val;
                    newNode->next = NULL;
                    addr->next = newNode;
                    
                    goto label1;
                }
            }
        label1:
            ;
        }
        ++index;
    }
    
    //
    keyIndex = zhashCode(akey, strlen(akey)) % newSize;
    zmapNode* addr = newNodeData + keyIndex;

    for (; addr != NULL; addr = addr->next) {
        if (addr->key == NULL) {
            addr->key = akey;
            addr->val = aval;
            
            goto label2;
        }
        if (csIsEqual(akey, addr->key)) {
            addr->val = aval;

            goto label2;
        }
        if (addr->next == NULL) {
            zmapNode* newNode = (zmapNode*)malloc(sizeof(zmapNode));
            newNode->key = akey;
            newNode->val = aval;
            newNode->next = NULL;
            addr->next = newNode;

            goto label2;
        }
    }
    
label2:
    ;
    
    self->data = newNodeData;
}

void* zmapGet(ZMap* self, char* key) {
    if (self == NULL) {
        return NULL;
    }
    int index = zhashCode(key, strlen(key)) % self->size;
    zmapNode* node = self->data + index;
    
    for (; node != NULL && node->key != NULL; node = node->next) {
        if (csIsEqual(key, node->key)) {
            return node->val;
        }
    }
    
    return NULL;
}

//值默认为string
char* zmapToString(ZMap* self) {
    if (self == NULL) {
        return NULL;
    }
    
    char* str = (char*)malloc(1);
    str = csAppend(NULL, "{");
    
    int capacity = self->size;
    for (int i = 0; i < capacity; i++) {
        zmapNode* node = self->data + i;
        for (; node != NULL && node->key != NULL; node = node->next) {
            //k
            char* key = node->key;
            str = csAppend(str, "\"");
            str = csAppend(str, key);
            str = csAppend(str, "\"");
            str = csAppend(str, ":");

            //v
            ZStruct* val = node->val;
            int type = val->type;
            switch (type) {
                case typeZArray:
                    str = csAppend(str, zarrayToString(((ZArray*)val)));
                    str = csAppend(str, ",");
                    break;
                case typeZMap:
                    str = csAppend(str, zmapToString(((ZMap*)val)));
                    str = csAppend(str, ",");
                    break;
                default:
                    str = csAppend(str, "\"");
                    str = csAppend(str, (char*)val);
                    str = csAppend(str, "\",");
                    break;
            }
        }
    }
    
    str = csReplaceCharAtLast(str, '}');
    str = csAppend(str, "\0");
    return str;
}

void main_zmap() {
//void main() {

    char* str = "fafafafafaf";
    printf("%p\n", str);
    
    ZMap* map = zmapInit();
    zmapPut(map, "fa", "valuefa");
    zmapPut(map, "fa2", str);
    
    str = "test";

    printf("%s, %p\n", zmapGet(map, "fa2"), str);
    
    ZMap* innerMap = zmapInit();
    zmapPut(innerMap, "innerk", "innerv");
    zmapPut(map, "innerMap", innerMap);
    
    uint64_t s = ztimeSince1970();
    for (int i = 0; i < 0; i++) {
        zmapPut(map, csAppendInt("tiankong", i), "value");
    }

    printf("%llu, %s\n", ztimeSince1970() - s, zmapGet(map, "tiankong3333"));
    printf("map -> json \n %s \n", zmapToString(map));
}