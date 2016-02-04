//
//  ZSearch.h
//  ZCDB
//
//  Created by zhangxinwei on 16/2/2.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#include "ZC.h"

int zbinarySearchForUlong(ulong data[], ulong theData, int start, int end);

int zbinarySearchForOutterUnlong(ulong data[], ulong theData, int start, int end);

int zbinarySearchNearPosForString(char* data, char* theData, int start, int end, int charSize);


