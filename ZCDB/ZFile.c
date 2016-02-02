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