//
//  CString.c
//  oc_demo
//
//  Created by zhangxinwei on 15/12/8.
//  Copyright © 2015年 zhangxinwei. All rights reserved.
//

#include "CString.h"
#include <sys/malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <ctype.h>
#include <stdlib.h>
#include "ZC.h"


//end：字节索引，不含end
char* csSliceByEndByteIndex(char* self, int end, int len) {
    char* s = (char*)malloc(end + 1);
    memset(s, 0, end+1);
    if (len == 0) {//至始
        memcpy(s, self, end);
    }else {
        memcpy(s, self, len);
    }
    
    return s;
}

//begin：字节索引
char* csSliceByBeginByteIndex(char* self, int begin, int len) {
    size_t selfLen = strlen(self);
    char* s = (char*)malloc(selfLen - begin + 1);
    memset(s, 0, selfLen - begin + 1);
    if (len == 0) {//至尽
        memcpy(s, self + begin, selfLen - begin);
    }else {
        memcpy(s, self + begin, len);
    }
    
    return s;
}

//  sunday算法，返字节索引
int csSearch(char* self, char* smallChars) {
    size_t bigCharsLen = strlen(self);
    size_t smallCharLen = strlen(smallChars);
    size_t ibig = 0;
    size_t ismall = 0;
    
    size_t start = 0;
    
    // 0 不在，1始
    int cmpArray[256];
    memset(cmpArray, 0, sizeof(cmpArray));
    for (int i = 0; i < smallCharLen; i++) {
        cmpArray[ smallChars[i] ] = i + 1;
    }
    
label:
    while (ibig < bigCharsLen) {
        if (self[ibig] == smallChars[ismall]) {
            ++ibig;
            ++ismall;
        }else {
            //若不等，在长小串后一个字符，查此字符是否在小串里，查从后往前
            unsigned char checkChar = self[start + smallCharLen];
            if (cmpArray[checkChar] > 0) {
                ibig = start + smallCharLen;
                ibig -= (cmpArray[checkChar] - 1); //原从1始
                start = ibig;
                
                ismall = 0;
                goto label;
            }
            
            ibig = start + smallCharLen;
            ibig += 1;
            start = ibig;
            
            ismall = 0;
        }
        
        if (ismall == smallCharLen - 1) {
            return (int)(ibig - ismall);
        }
    }
    
    return -1;
}

bool csIsEqual(char* self, char* str) {
    if (self == NULL || str == NULL) {
        return false;
    }
    size_t strLen = strlen(self);
    if (strLen != strlen(str)) {
        return false;
    }
    
    for (int i = 0; i < strLen; i++) {
        if (*self++ != *str++) {
            return false;
        }
    }
    
    return true;
}

char* csLineByClientFd(int clientFd) {
    char c = '\0';
    ssize_t n;
    
    char* buf = (char*)malloc(1024);
    
    while (c != '\n') {
        n = recv(clientFd, &c, 1, 0);
        *buf++ = c;
    }
    *buf = '\0';
    
    return buf;
}

int csIndexPrefixSkipSpace(char* self, char* prefix) {
    size_t prefixLen = strlen(prefix);
    int index = 0;
    char* addr = self;
    // skip space
    while (isspace((char)*self)){
        ++self;
    }
    index = (int)(self - addr);
    
    for (int i = 0; i < prefixLen; i++) {
        if (*prefix++ != *self++) {
            printf("%c\n", (char)*self);
            return -1;
        }
    }
    
    return index;
}

//malloc
char* csReplaceCharAtLast(char* self, char c) {
    if (self == NULL) {
        return "";
    }
    size_t selfLen = strlen(self);

    *(self + (selfLen-1)) = c;
    
    return self;
}

char* csAppendChar(char* self, char ch) {
    if (self == NULL) {
        return "";
    }
    size_t selfLen = strlen(self);
    
    char* newS = (char*)malloc(selfLen + 1);
    if (newS == NULL) {
        printf("malloc no mem");
        return NULL;
    }
    
    memcpy(newS, self, selfLen);
    
    *(newS+selfLen) = ch;
    
    return newS;
}


//copy "/"
char* csPathAppendComponent(const char* self, const char* str) {
    size_t selfLen = 0;
    if (self != NULL) {
        selfLen = strlen(self);
    }
    size_t strLen = 0;
    if (str != NULL) {
        strLen = strlen(str);
    }
    
    char* newS = (char*)malloc(selfLen + strLen + 1 + 1);
    if (newS == NULL) {
        printf("malloc no mem");
        return NULL;
    }
    
    void* temp = newS;
    
    for (int i = 0; i < selfLen; i++) {
        *newS++ = *self++;
    }
    
    *newS++ = '/';
    
    for (int i = 0; i < strLen; i++) {
        *newS++ = *str++;
    }
    
    *newS = '\0';
    
    return temp;

}

//copy
char* csAppend(const char* self, const char* str) {
    size_t selfLen = 0;
    if (self != NULL) {
        selfLen = strlen(self);
    }
    size_t strLen = 0;
    if (str != NULL) {
        strLen = strlen(str);
    }
    
    char* newS = (char*)malloc(selfLen + strLen + 1);
    if (newS == NULL) {
        printf("malloc no mem");
        return NULL;
    }

    void* temp = newS;
    
    for (int i = 0; i < selfLen; i++) {
        *newS++ = *self++;
    }
    
    for (int i = 0; i < strLen; i++) {
        *newS++ = *str++;
    }
    
    *newS = '\0';
    
    return temp;
}

char* csAppendInt(char* self, int intVal) {
    char* str = csIntToString(intVal);
    size_t selfLen = strlen(self);
    size_t strLen = strlen(str);
    
    char* newS = (char*)malloc(selfLen + strLen + 1);
    if (newS == NULL) {
        printf("malloc no mem");
        return NULL;
    }
    
    void* temp = newS;
    
    for (int i = 0; i < selfLen; i++) {
        *newS++ = *self++;
    }
    
    for (int i = 0; i < strLen; i++) {
        *newS++ = *str++;
    }
    
    *newS = '\0';
    
//    free(str);
//    free(self);
    return temp;
}

char* csIntToString(int intVal) {
    int temp = intVal;
    char* str = (char*)malloc(12);
    size_t count = 0;
    if (temp < 0) {
        intVal = -intVal;
    }
    
    do {
        ++count;
        *str++ = intVal % 10 + '0';
        intVal = intVal / 10;
    } while (intVal > 0);
    
    if (temp < 0) {
        *str++ = '-';
        ++count;
    }
    
    
    //reverse
    char* c = (char*)malloc(count+1);
    void* addr = c;
    while (count > 0) {
        *c++ = *(--str);
        --count;
    }
    
    *c = '\0';
    
    free(str);
    return addr;
}


int csToInt(char* self) {
    
    return 0;
}

//是前缀否
bool csIsPrefix(char* self, char* prefix) {
    if (self == NULL || prefix == NULL) {
        return false;
    }

    size_t selfLen = strlen(self);
    size_t prefixLen = strlen(prefix);
    if (selfLen < prefixLen) {
        return false;
    }
    
    for (int i = 0; i < prefixLen; i++) {
        if (*(self+i) != *(prefix+i)) {
            return false;
        }
    }
    
    return true;
}

//分隔
char* csSeparateToEnd(char* self, char* c) {
    int charIndex = csSearch(self, c);
    if (charIndex == -1) {
        return NULL;
    }
    return csSliceByBeginByteIndex(self, charIndex+1, 0);
}

char* csSeparateFromBegin(char* self, char* c) {
    int charIndex = csSearch(self, c);
    if (charIndex == -1) {
        return NULL;
    }
    return csSliceByEndByteIndex(self, charIndex, 0);
}

void test () {
    int test = 12306;
}

void main_cstring() {
//void main() {
    char* str = "{{{{{{{{";
    
    char tst[] = "{";
    printf("%s \n", csReplaceCharAtLast(tst, ']'));

    char* s2 = csAppend(str, "zhangafafafafa");

    printf("%s\n", s2);
    printf("%s\n", str);
    
    printf("%s\n", csIntToString(-24343434));
    
    printf("%s\n", csAppendInt(str, -4434343));
    
    printf("%d\n", csIndexPrefixSkipSpace("prefixffff", "pre"));
    
    printf("%d\n", csIsEqual("test1", "testc"));
    printf("%d\n", csIsEqual("张新伟", "张新伟b"));
    
    char* bigString = "丈夫福建省大方款式大方test";
    char* smallString = "test";
    
    int index = csSearch(bigString, smallString);
    printf("%d \n", index);
    
    printf("%c \n", bigString[1]);
    
    printf("%s \n", csSliceByEndByteIndex(bigString, 33, 0));
    
    printf("%s \n", csSliceByBeginByteIndex(bigString, 33, 0));
    
    printf("%s \n", csAppend("", "ccc"));
    
    printf("%s \n", csAppendChar("aa", 'c'));
}