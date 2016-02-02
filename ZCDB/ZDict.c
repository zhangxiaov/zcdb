//
//  ZDict.c
//  oc_demo
//
//  Created by zhangxinwei on 15/12/7.
//  Copyright © 2015年 zhangxinwei. All rights reserved.
//

#include "ZDict.h"
#include <sys/malloc.h>
#include <sys/socket.h>
#include "ZHash.h"

struct _ZDict {
    int8_t type;
    int typeSize;
    var data;
};


ZDict* zdictInit() {
    ZDict* dict = (ZDict*)malloc(sizeof(ZDict));
    
    return dict;
}


void zdictSet(ZDict* self, char* key, void* val) {

}

ZDict* zdictInitWithClientFd(int clientFd) {
    ZDict* dict = (ZDict*)malloc(sizeof(ZDict));
    
    return dict;
}

void* zdictGetString(ZDict* self, char* key) {
    void* val;
    
    return val;
}