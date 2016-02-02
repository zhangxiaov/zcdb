//
//  zepoll_apple.c
//  ZC
//
//  Created by 张新伟 on 16/1/23.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#include "zepoll.h"

int faio__gettime_monotonic(struct timespec *spec) {
    uint64_t val;
    
    val = mach_absolute_time();
    spec->tv_sec = val / (uint64_t) 1e9;
    spec->tv_nsec = val % (uint64_t) 1e9;
    
    return 0;
}

ZSocketHandle* zsocketHandleInit() {
    ZSocketHandle* self = (ZSocketHandle*)malloc(sizeof(ZSocketHandle));
    ZSocketNode* node = (ZSocketNode*)malloc(sizeof(ZSocketNode));
    self->node = *node;
    
    return self;
}

ZSocketQueue* zsocketQueueInit() {
    int kq = kqueue();
    
    if (kq == -1)
        return NULL;
    
    ZSocketQueue* self = (ZSocketQueue*)malloc(sizeof(ZSocketQueue));
    self->node = (ZSocketNode*)malloc(sizeof(ZSocketNode));
    self->queuefd = kq;
    
    //node init
    self->node->next = self->node;
    self->node->prev = self->node;
    
    return self;
}

void zsocketQueueClear(ZSocketQueue* self) {
    close(self->queuefd);
    self->queuefd = -1;
}

int zsocketQueueISEmpty(ZSocketNode* node) {
    return node == node->prev;
}

static ZSocketNode* zsocketQueueHeader(ZSocketQueue* self) {
    return self->node->next;
}

void zsocketQueueDelNode(ZSocketQueue* self) {
    ZSocketNode* node = self->node;
    node->next->prev = node->prev;
    node->prev->next = node->next;
    
    node->prev = node;
    node->next = node;
}

void zsocketQueueEpoll(ZSocketQueue* self, double timeOut) {
    struct kevent events[256]; /* 8 kB */
    ZSocketHandle *handle;
    ZSocketNode *node;
    struct timespec before;
    struct timespec after;
    struct timespec diff;
    struct timespec *pts;
    struct timespec ts;
    unsigned int maxevents;
    unsigned int revents;
    int op;
    int i;
    int n;
    
    /* Silence compiler warning. */
    before.tv_nsec = 0;
    before.tv_sec = 0;
    
    maxevents = sizeof(events) / sizeof(events[0]);
    
    n = 0;
    
    while (!zsocketQueueISEmpty(self->node)) {
        node = zsocketQueueHeader(self);
        handle = CONTAINER_OF(node, ZSocketHandle, node);

        //node remove
        node->next->prev = node->prev;
        node->prev->next = node->next;
        node->prev = node;
        node->next = node;
        
        if ((handle->events & POLLIN) != (handle->revents & POLLIN)) {
            if (handle->events & POLLIN)
                op = EV_ADD | EV_ENABLE;
            else
                op = EV_DELETE | EV_DISABLE;
            
            EV_SET(events + n, handle->fd, EVFILT_READ, op, 0, 0, handle);
            
            if (maxevents == (unsigned int) ++n) {
                kevent(self->queuefd, events, n, NULL, 0, NULL);
                n = 0;
            }
        }
        
        if ((handle->events & POLLOUT) != (handle->revents & POLLOUT)) {
            if (handle->events & POLLOUT)
                op = EV_ADD | EV_ENABLE;
            else
                op = EV_DELETE | EV_DISABLE;
            
            EV_SET(events + n, handle->fd, EVFILT_WRITE, op, 0, 0, handle);
            
            if (maxevents == (unsigned int) ++n) {
                kevent(self->queuefd, events, n, NULL, 0, NULL);
                n = 0;
            }
        }
    }
    
    if (n != 0)
        kevent(self->queuefd, events, n, NULL, 0, NULL);
    
    if (timeOut < 0)
        pts = NULL;
    else if (timeOut == 0) {
        ts.tv_nsec = 0;
        ts.tv_sec = 0;
        pts = &ts;
    }
    else {
        ts.tv_nsec = (unsigned long) (timeOut * 1e9) % 1000000000UL;
        ts.tv_sec = (unsigned long) timeOut;
        pts = &ts;
    }
    
    if (pts != NULL)
        if (faio__gettime_monotonic(&before))
            abort();
    
    for (;;) {
        n = kevent(self->queuefd, NULL, 0, events, maxevents, pts);
        if (n == 0)
            return;
        
        if (n == -1) {
            if (errno == EINTR)
                goto update_timeout;
            else
                abort();
        }
        
        for (i = 0; i < n; i++) {
            handle = (ZSocketHandle*)events[i].udata;
            revents = 0;
            
            if (events[i].filter == EVFILT_READ)
                revents |= POLL_IN;
            if (events[i].filter == EVFILT_WRITE)
                revents |= POLLOUT;
            if (events[i].flags & EV_ERROR)
                revents |= POLLERR;
            if (events[i].flags & EV_EOF)
                revents |= POLLHUP;
            
            handle->cb(self, handle, revents);
        }
        
        /* We read as many events as we could but there might still be more.
         * Poll again but don't block this time.
         */
        if (maxevents == (unsigned int) n) {
            ts.tv_nsec = 0;
            ts.tv_sec = 0;
            pts = &ts;
            continue;
        }
        
        return;
        
    update_timeout:
        if (pts == NULL)
            continue;
        
        if (ts.tv_sec == 0 && ts.tv_nsec == 0)
            return;
        
        if (faio__gettime_monotonic(&after))
            abort();
        
        FAIO_TIMESPEC_SUB(&after, &before, &diff);
        FAIO_TIMESPEC_SUB(&ts, &diff, &ts);
        
        if (ts.tv_sec < 0)
            return;
        
        before = after;
    }
}

bool zsocketQueueAdd(ZSocketQueue* self, ZSocketHandle* handle,int fd, CB cb, unsigned int events) {
    events &= POLLIN | POLLOUT;
    events |= POLLERR | POLLHUP;
    
    //node append
    ZSocketNode* q = self->node;
    ZSocketNode* n = &handle->node;
    q->prev->next = n;
    n->prev = q->prev;
    n->next = q;
    q->prev = n;
    
    handle->cb = cb;
    handle->fd = fd;
    handle->events = events;
    handle->revents = 0;
    
    return true;
}

bool zsocketQueueMod(ZSocketQueue* self, ZSocketHandle* handle, unsigned int events) {
    events &= POLLIN | POLLOUT;
    events |= POLLERR | POLLHUP;
    handle->revents = handle->events;
    handle->events = events;
    
    if (handle->events != handle->revents) {
        if (zsocketQueueISEmpty(&handle->node)) {
            //node append
            ZSocketNode* q = self->node;
            ZSocketNode* n = &handle->node;
            
            q->prev->next = n;
            n->prev = q->prev;
            n->next = q;
            q->prev = n;
            
        }
    }
    
    return true;
}

bool zsocketQueueDel(ZSocketQueue* self, ZSocketHandle* handle) {
    struct kevent events[2];
    int fd;
    
    handle->revents = 0;
    handle->events = 0;
    
    if (!zsocketQueueISEmpty(&handle->node)) {
        //node remove
        ZSocketNode* node = &handle->node;
        node->next->prev = node->prev;
        node->prev->next = node->next;
        node->prev = node;
        node->next = node;
    }
    
    fd = handle->fd;
    EV_SET(events + 0, fd, EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, handle);
    EV_SET(events + 1, fd, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, handle);
    
    return kevent(self->queuefd, events, 2, NULL, 0, NULL);
}