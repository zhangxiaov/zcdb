//
//  ZHttpServer.c
//  oc_demo
//
//  Created by zhangxinwei on 15/12/9.
//  Copyright © 2015年 zhangxinwei. All rights reserved.
//

#include "ZHttpServer.h"
#include <sys/malloc.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "CString.h"
#include <sys/errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <ctype.h>
#include "ZDict.h"


static int httpTimeOut = 30;



ZHttpServer* zhttpServerInit(int port) {
    ZHttpServer* httpServer = (ZHttpServer*)malloc(sizeof(ZHttpServer));
    httpServer->type = ZHTTPSERVER;
    httpServer->typeSize = sizeof(ZHttpServer);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("%s\n", strerror(errno));
        return NULL;
    }
    httpServer->fd = fd;
    
    struct addrinfo hints, *serverInfo;
    bzero(&hints, sizeof(hints));
    
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    int error_getaddr = getaddrinfo(NULL, csIntToString(port), &hints, &serverInfo);
    if (error_getaddr) {
        printf("%s\n", strerror(errno));
        return NULL;
    }
    
    int error_bind = bind(fd, serverInfo->ai_addr, serverInfo->ai_addrlen);
    if (error_bind < 0) {
        printf("%s\n", strerror(errno));
        return NULL;
    }
    
    if (listen(fd, 10) == -1) {
        printf("%s\n", strerror(errno));
        return NULL;
    }

    
    return httpServer;
}

void zhttpServerBadRequest(int clientFd, int state) {
    char buf[1024];

    switch (state) {
        case 400 :
            sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
            send(clientFd, buf, sizeof (buf), 0);
            sprintf(buf, "Content-type: text/html\r\n");
            send(clientFd, buf, sizeof (buf), 0);
            sprintf(buf, "\r\n");
            send(clientFd, buf, sizeof (buf), 0);
            sprintf(buf, "<P>Your browser sent a bad request, ");
            send(clientFd, buf, sizeof (buf), 0);
            sprintf(buf, "such as a POST without a Content-Length.\r\n");
            send(clientFd, buf, sizeof (buf), 0);
            break;
        case 500:
            sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
            send(clientFd, buf, strlen(buf), 0);
            sprintf(buf, "Content-type: text/html\r\n");
            send(clientFd, buf, strlen(buf), 0);
            sprintf(buf, "\r\n");
            send(clientFd, buf, strlen(buf), 0);
            sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
            send(clientFd, buf, strlen(buf), 0);
        case 501:
            sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
            send(clientFd, buf, strlen(buf), 0);
//            sprintf(buf, SERVER_STRING);
            send(clientFd, buf, strlen(buf), 0);
            sprintf(buf, "Content-Type: text/html\r\n");
            send(clientFd, buf, strlen(buf), 0);
            sprintf(buf, "\r\n");
            send(clientFd, buf, strlen(buf), 0);
            sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
            send(clientFd, buf, strlen(buf), 0);
            sprintf(buf, "</TITLE></HEAD>\r\n");
            send(clientFd, buf, strlen(buf), 0);
            sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
            send(clientFd, buf, strlen(buf), 0);
            sprintf(buf, "</BODY></HTML>\r\n");  
            send(clientFd, buf, strlen(buf), 0);
        default:
            break;
    }
}

void zhttpServerSend(ZHttpServer* self, char* msg, int clientFd) {
    if (msg == NULL) {
        msg = "";
    }
    
    size_t len = strlen(msg);
    send(clientFd, msg, len, 0);
}

//void zhttpServerRegisterPathHandle(ZHttpServer* self, char* path, void (*func)(ZHttpServer*, int clientFd)) {
//    self->dictPath = NULL;
//}

void* zhttpServerGetFunc(ZHttpServer* self, char* path) {
    
    return 0;
}

void zhttpServerHandle(ZHttpServer* self, int clientFd) {
    
//    ZDict* headerDict = zdictInitWithClientFd(clientFd);
//    ZDict* dictPath = self->dictPath;
    
    char* path = self->path;
    
//    (void (*)(ZHttpServer*, int clientFd)) func = zdictGetString(self->dictPath, path);
//    func
}

void zhttpServerAccept(ZHttpServer* self, ZDict* pathHandle) {
    int clientFd = 0;
    pthread_t newthread;
    struct sockaddr_storage remoteAddr;

    while (1) {
        clientFd = accept(self->fd, (struct sockaddr *)&remoteAddr, &(socklen_t){sizeof(remoteAddr)});
        if (clientFd == -1) {
            printf("%s\n", strerror(errno));
            return;
        }
        
        //子线程处理http 请求
//        if (pthread_create(&newthread, NULL, zhttpServerHandle, self, (void*) clientFd) != 0)
//            perror("pthread_create");
    }
}