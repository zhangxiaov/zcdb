//
//  ZC.h
//  ZC
//
//  Created by 张新伟 on 16/1/2.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#ifndef ZC_h
#define ZC_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>

#define zptr unsigned long*

typedef unsigned long ulong;

typedef struct _ZStruct ZStruct;

struct _ZStruct {
    int type;
    int typeSize;
    zptr data;
};

#define bool int
#define true 1
#define false 0

#define typeZStruct 0xffff
#define typeByte    0xfffe
#define typeInt     0xfffd
#define typeLong    0xfffc
#define typeFloat   0xfffb
#define typeDouble  0xfffa
#define typeString  0xffef

#define typeZMap    0xffee
#define typeZJson   0xffed
#define typeZArray  0xffec
#define typeZStack  0xffeb

#define typePtr 5


int isWhiteSpace(char ch);

void throw(char* exception);

#endif /* ZC_h */
