//
//  ZJson.h
//  oc_demo
//
//  Created by zhangxinwei on 15/12/7.
//  Copyright © 2015年 zhangxinwei. All rights reserved.
//

#ifndef ZJson_h
#define ZJson_h

#include <stdio.h>
#include "ZC.h"


#define ZJSON 1
#define ZJSONARRAY 2
#define ZI 3
#define ZS 4

typedef struct _ZJson ZJson;

struct _ZJson {
    int type;
    int typeSize;
    int size;
    int count;
    void* data;
    char* originString;
};
typedef struct _zi zi;
typedef struct _zs zs;
typedef struct _KV KV;
typedef struct _CZ CZ;

#define zi(intVal) \
    &((struct _zi){.type=ZI,.val=intVal})
#define zs(strVal) \
    &((struct _zs){.type=ZS,.val=strVal, .valLen=strlen(strVal)})


#define var2(left, right) __typeof__(right) left = (right)


struct _zi {
    char type;
    int typeSize;
    int val;
};

struct _zs {
    char type;
    int typeSize;
    char* val;
    int valLen;
};

char* zjsonToString(ZJson* json);


#define idnumber 0
#define idbool 1
#define idstring 2
#define idnull 3
#define idobjbegin 4
#define idobjend 5
#define idlistbegin 6 //'['
#define idlistend 7
#define idcolon 8 //':'
#define idcomma 9 //','
#define idend 10 //json str end

#define STATUS_EXPECT_END_DOCUMENT 0x0002
#define STATUS_EXPECT_BEGIN_OBJECT 0x0004
#define STATUS_EXPECT_END_OBJECT 0x0008
#define STATUS_EXPECT_OBJECT_KEY 0x0010
#define STATUS_EXPECT_OBJECT_VALUE 0x0020
#define STATUS_EXPECT_COLON 0x0040
#define STATUS_EXPECT_COMMA 0x0080
#define STATUS_EXPECT_BEGIN_ARRAY 0x0100
#define STATUS_EXPECT_END_ARRAY 0x0200
#define STATUS_EXPECT_ARRAY_VALUE 0x0400
#define STATUS_EXPECT_SINGLE_VALUE 0x0800

#define TYPE_OBJECT_KEY 1
#define TYPE_ARRAY 2

#define zjsonNumber char*
#define zjsonBoolean char*

ZJson* zjsonInitWithString(char* jsonStr);

#endif /* ZJson_h */
