//
//  epoll.h
//  ZC
//
//  Created by 张新伟 on 16/1/23.
//  Copyright © 2016年 张新伟. All rights reserved.
//

// copy from https://github.com/bnoordhuis/faio

#include "ZC.h"
#include <netinet/in.h>
#include <sys/filio.h>
#include <sys/ioctl.h>
#include <sys/event.h>
#include <sys/poll.h>
#include <mach/mach_time.h>


#define CONTAINER_OF(ptr, type, member)                                       \
((type *) ((char *) (ptr) - (unsigned long) &((type *) 0)->member))

/* Makes c = a - b. Assumes tv_sec is signed in the case that a < b. */
#define FAIO_TIMESPEC_SUB(a, b, c)                                            \
do {                                                                        \
    long nsec = (long) (a)->tv_nsec - (long) (b)->tv_nsec;                    \
    long sec = (long) (a)->tv_sec - (long) (b)->tv_sec;                       \
    if (nsec < 0) {                                                           \
        nsec += 1000000000UL;                                                   \
        sec -= 1;                                                               \
    }                                                                         \
    (c)->tv_nsec = nsec;                                                      \
    (c)->tv_sec = sec;                                                        \
}while (0)

#define E(expr)                                                               \
do {                                                                        \
    errno = 0;                                                                \
    do { expr; } while (0);                                                   \
    if (errno) {                                              \
        exit(42); \
    }\
}while (0)


typedef struct _ZSocketQueue ZSocketQueue;
typedef struct _ZSocketNode ZSocketNode;
typedef struct _ZSocketHandle ZSocketHandle;
typedef void (*CB)(ZSocketQueue *queue, ZSocketHandle* handle, unsigned int revents);

struct _ZSocketQueue {
    ZSocketNode* node;
    int queuefd;
};

struct _ZSocketNode {
    ZSocketNode* prev;
    ZSocketNode* next;
};

struct _ZSocketHandle {
    ZSocketNode node;
//    void (*cb)(ZSocketQueue *queue, ZSocketHandle* handle, unsigned int revents);
    CB cb;
    unsigned int revents;
    unsigned int events;
    int fd;
    void (*parseData)(ZSocketHandle* handle);
};


//handle
ZSocketHandle* zsocketHandleInit();

// server
ZSocketQueue* zsocketQueueInit();

void zsocketQueueClear(ZSocketQueue* self);

void zsocketQueueEpoll(ZSocketQueue* self, double timeOut);

bool zsocketQueueAdd(ZSocketQueue* self, ZSocketHandle* handle,int fd, CB cb, unsigned int events);

bool zsocketQueueMod(ZSocketQueue* self, ZSocketHandle* handle, unsigned int events);

bool zsocketQueueDel(ZSocketQueue* self, ZSocketHandle* handle);

bool zsocketQueueISEmpty(ZSocketNode* node);

//client
typedef struct _zclient zclient;

struct _zclient {
    ZSocketHandle fh;
    char* dataIn;
    size_t dataInLen;
    char* dataOut;
    size_t dataOutLen;
};

void ztcpClientCB(ZSocketQueue *loop,ZSocketHandle *fh, unsigned int revents);
int ztcpClientRead(zclient* self, ZSocketQueue* loop);
int ztcpClientWrite(zclient* self, ZSocketQueue* loop);
void zioNB(int fd);
int zsocketNB(int family, int type, int proto);
int zacceptNB(int sfd, struct sockaddr *saddr, socklen_t *slen);
void zacceptCB(ZSocketQueue *loop,ZSocketHandle *fh, unsigned int revents);
