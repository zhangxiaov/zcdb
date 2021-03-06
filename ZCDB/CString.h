//
//  CString.h
//  oc_demo
//
//  Created by zhangxinwei on 15/12/8.
//  Copyright © 2015年 zhangxinwei. All rights reserved.
//

#ifndef CString_h
#define CString_h

#include <stdio.h>
#include "ZC.h"

int csIndexPrefixSkipSpace(char* self, char* prefix);
char* csLineByClientFd(int clientFd);
char* csReplaceCharAtLast(char* self, char c);
char* csAppend(const char* self, const char* str);
char* csAppendInt(char* self, int intVal);
char* csAppendChar(char* self, char c);
char* csIntToString(int intVal);
bool csIsEqual(const char* self,const char* str);
int csToInt(char* self);

//是前缀否
bool csIsPrefix(char* self, char* prefix);
    
//分隔
char* csSeparateToEnd(char* self, char* c);
char* csSeparateFromBegin(char* self, char* c);


char* csPathAppendComponent(const char* self, const char* str);


//
char* csSearchValByCVCForJson(char* self, char* key, int variableMaxSize);

//constant2 可能是个数组 是xx或是cc 返回variable的内容
char* csSearchLikeByCVC(char* self, char* constant1, int variableMaxSize, void* constant2);

char* csSearchValByCVCForJson(char* self, char* key, int variableMaxSize);
#endif /* CString_h */
