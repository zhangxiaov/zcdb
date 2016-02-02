//
//  tcpServer.c
//  ZC
//
//  Created by 张新伟 on 16/1/23.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#include "zepoll.h"
void zacceptCB(ZSocketQueue *loop,ZSocketHandle *fh, unsigned int revents) {
    zclient *c;
    int fd;
    
    assert(revents == POLLIN);
    
    while (-1 != (fd = zacceptNB(fh->fd, NULL, NULL))) {
        c = calloc(1, sizeof(*c));
        c->fh.parseData = fh->parseData;
        if (c == NULL)
            abort();
        
        if (!zsocketQueueAdd(loop, &c->fh, fd, ztcpClientCB, POLLIN))
            abort();
    }
    
    assert(errno == EAGAIN);
}

void ztcpClientCB(ZSocketQueue *loop,ZSocketHandle *fh, unsigned int revents) {
    zclient *c = CONTAINER_OF(fh, zclient, fh);
    
    if (revents & (POLLERR | POLLHUP))
        goto err;
    
    if (revents & POLLIN)
        if (!ztcpClientRead(c, loop))
            goto err;
    
    if (revents & POLLOUT) {
        if (!ztcpClientWrite(c, loop))
            goto err;
    }
    
    return;
    
err:
    zsocketQueueDel(loop, fh);
    close(c->fh.fd);
    free(c);
}

int ztcpClientRead(zclient* self, ZSocketQueue* loop) {
    char buf[1024];
    ssize_t n;
    
    do
        n = read(self->fh.fd, buf, sizeof(buf));
    while (n == -1 && errno == EINTR);
    
    self->dataIn = buf;
    self->dataInLen = sizeof(buf);
    
    ZSocketHandle* handle = &self->fh;
    handle->parseData(handle);
    
    return zsocketQueueMod(loop, &self->fh, POLLOUT);
}

int ztcpClientWrite(zclient* self, ZSocketQueue* loop) {
    ssize_t n;

    do
        n = write(self->fh.fd, self->dataOut, self->dataOutLen);
    while (n == 0 && errno == EINTR);
    
    return zsocketQueueMod(loop, &self->fh, POLLIN);
}

void zioNB(int fd) {
    int on = 1;
    E(ioctl(fd, FIONBIO, &on));
}

int zsocketNB(int family, int type, int proto) {
    int fd;
    
    fd = socket(family, type, proto);
    if (fd != -1)
        zioNB(fd);
    
    return fd;
}

int zacceptNB(int sfd, struct sockaddr *saddr, socklen_t *slen) {
    int fd;
    
    fd = accept(sfd, saddr, slen);
    if (fd != -1)
        zioNB(fd);
    
    return fd;
}

int ztcpServerInit(unsigned short port) {
    struct sockaddr_in sin;
    int fd;
    int on;
    
    E(fd = zsocketNB(AF_INET, SOCK_STREAM, 0));
    on = 1;
    E(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)));
    
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY;
    E(bind(fd, (const struct sockaddr *) &sin, sizeof(sin)));
    E(listen(fd, 1024));
    
    return fd;
}

