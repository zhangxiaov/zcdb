//
//  ZArray.h
//  oc_demo
//
//  Created by 张新伟 on 15/12/13.
//  Copyright (c) 2015年 zhangxinwei. All rights reserved.
//

#ifndef __oc_demo__ZArray__
#define __oc_demo__ZArray__

#define PP_NARG(...) PP_NARG_(__VA_ARGS__,PP_RSEQ_N()) /* 求参数个数 */
#define PP_NARG_(...) PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
__1,__2,__3,__4,__5,__6,__7,__8,__9,_10,\
_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,\
_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,\
_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,\
_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,\
_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,\
_61,_62,_63,N,...) N
#define PP_RSEQ_N() \
63,62,61,60,\
59,58,57,56,55,54,53,52,51,50,\
49,48,47,46,45,44,43,42,41,40,\
39,38,37,36,35,34,33,32,31,30,\
29,28,27,26,25,24,23,22,21,20,\
19,18,17,16,15,14,13,12,11,10,\
9,8,7,6,5,4,3,2,1,0

#include <stdio.h>
#include "ZC.h"
#include "ZMap.h"

typedef struct _ZArray ZArray;
// data 仅存 指针类型
struct _ZArray {
    int type;
    int typeSize;
    zptr data;
    int len;
    int pos;
    bool isDynamic;
};
ZArray* zarrayInit(bool isDynamic);
ZArray* zarrayInitWithVal(int n, void* val1,...);
void zarrayAdd(ZArray* self, void* val);
void* zarrayGet(ZArray* self, int index);
char* zarrayToString(ZArray* self);
ZArray* zarrayFileNames(char* dirName);

//获取目录下文件名以preifx始 数组
ZArray* zarrayFileNameWithPrefix(char* path, char* prefix);

#endif /* defined(__oc_demo__ZArray__) */
