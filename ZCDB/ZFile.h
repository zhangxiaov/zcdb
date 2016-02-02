//
//  ZFile.h
//  ZC
//
//  Created by zhangxinwei on 16/1/28.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#ifndef ZFile_h
#define ZFile_h

#include <stdio.h>
#include "ZC.h"

ZArray* zfileNamesByPath(char* dirPath);

//void zfileCreate(char* fileName);
//
////读取len子节
//char* zfileRead(char* self, int startPos, int len);
//
////
//bool zfileWrite(char* self, int startPos, void* valuePtr);
//
////追加指定字节的文件
//void zfileWriteStringSpecifySize(char* self, char* strval, int size);
//
////指定位置指定字节的文件
//void zfileWriteStringSpecifySizeWithPos(char* self, char* strval, int size, unsigned long pos);
//
////追加指定字节的文件 8byte
//void zfileWriteULongSpecifySize(char* self, unsigned long intval, int size);
//
////指定位置指定字节的文件 8byte
//void zfileWriteULongSpecifySizeWithPos(char* self, unsigned long intval, int size, unsigned long pos);
//
////书长,self:名
//unsigned long zfileSize(char* self);
//
////书存
//bool zfileIsExist(char* self, char* dir);
//
//// 二进制追加写ul
//void zfileWriteInt(char* self, unsigned long v);
//
//// 二进制指定位置写ul
//void zfileWriteIntWithPos(char* self, unsigned long v, unsigned long pos);
//
//void zfileDestory(char* self);
//
////创建指定大小的孔文件
//void zfileCreateEmptySpecifySize(char* fileName, int size);

#endif /* ZFile_h */
