//
//  ZSort.h
//  ZCDB
//
//  Created by zhangxinwei on 16/2/3.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#include "ZC.h"

void zsortForStringPairAsString(ulong* id, void* data, int left, int right);

void zsortForStringPairAsNumber(void* id, void* data, int left, int right);

//quicksort for string as num
void zsortForStringAsNumber(void* data, int left, int right);

//quicksort for string as string
void zsortForStringAsString(void* data, int left, int right);