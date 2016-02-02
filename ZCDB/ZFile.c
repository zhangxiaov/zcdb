//
//  ZFile.c
//  ZC
//
//  Created by zhangxinwei on 16/1/28.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#include "ZFile.h"

void zfileCreateEmptySpecifySize(char* fileName, int size) {
    int ret = truncate(fileName, size);
    if (ret != 0) {
        printf("failed when truncate\n");
    }
}

//读全部数据
char* zfileReadAllData(char* filePath, ulong* size) {
    FILE* fp = fopen(filePath, "r");
    if (fp == NULL) {
        return NULL;
    }
    
    fseek(fp, 0, SEEK_END);
    ulong len = ftell(fp);
    *size = len;
    char* buf = (char*)malloc(len);
    if (buf == NULL) {
        printf("mem alloc failed!");
        fclose(fp);
        return NULL;
    }
    
    fread(buf, len, 1, fp);
    fclose(fp);
    fp = NULL;
    
    return buf;
}