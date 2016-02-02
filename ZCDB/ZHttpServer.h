//
//  ZHttpServer.h
//  oc_demo
//
//  Created by zhangxinwei on 15/12/9.
//  Copyright © 2015年 zhangxinwei. All rights reserved.
//

#ifndef ZHttpServer_h
#define ZHttpServer_h

#include <stdio.h>
#include "ZDict.h"

#define ZHTTPSERVER 5

#define NOTFOUND 404


typedef struct _ZHttpServer ZHttpServer;

struct _ZHttpServer {
    char type;
    int typeSize;
    
    int fd;
    char* path;
    
    ZDict* dictPath;
    ZDict* dictParam;
};

ZHttpServer* zhttpServerInit(int port);

void zhttpServerHandle(ZHttpServer* self, int clientFd);

void zhttpServerRegisterPathHandle(ZHttpServer* self, char* path, void (*func)(ZHttpServer*));

void zhttpServerAccept(ZHttpServer* self, ZDict* pathHandle);

void zhttpServerSend(ZHttpServer* self, char* msg, int clientFd);

#endif /* ZHttpServer_h */
