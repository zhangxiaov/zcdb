//
//  ZDict.h
//  oc_demo
//
//  Created by zhangxinwei on 15/12/7.
//  Copyright © 2015年 zhangxinwei. All rights reserved.
//

#ifndef ZDict_h
#define ZDict_h

#include <stdio.h>
typedef struct _ZDict ZDict;
#define var void*

ZDict* zdictInit();
void zdictSet(ZDict* self, char* key, void* val);
ZDict* zdictInitWithClientFd(int clientFd);
void* zdictGetString(ZDict* self, char* key);

#endif /* ZDict_h */
