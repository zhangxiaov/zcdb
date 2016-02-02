//
//  ztcpServerStart.c
//  ZC
//
//  Created by 张新伟 on 16/1/23.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#include "zepoll.h"
#include "CString.h"


void ztcpServerParseData(ZSocketHandle* handle) {
    zclient* client = CONTAINER_OF(handle, zclient, fh);
    char* receiveData = client->dataIn;
    
    if (csIsEqual(receiveData, "hello")) {
        char* out = "hello\n";
        client->dataOut = out;
        client->dataOutLen = strlen(out);
    }else {
        char* out = "ok\n";
        client->dataOut = out;
        client->dataOutLen = strlen(out);
    }
}


int main_tcpserver() {
//int main() {
    E(signal(SIGPIPE, SIG_IGN));
    
    int server_fd = ztcpServerInit(1234);
    if (server_fd == -1)
        abort();
    
    ZSocketQueue* loop = zsocketQueueInit();
    if (loop == NULL) {
        abort();
    }
    
    ZSocketHandle* handle = zsocketHandleInit();
    handle->parseData = ztcpServerParseData;
//    ZSocketHandle handle;
    if (!zsocketQueueAdd(loop, handle, server_fd, zacceptCB, POLLIN)) {
        abort();
    }
    
    for (;;) {
        zsocketQueueEpoll(loop, -1);
    }
    
    //close
    zsocketQueueClear(loop);
    close(server_fd);
    
    return 0;
}


