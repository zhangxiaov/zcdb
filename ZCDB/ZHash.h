//
//  ZHash.h
//  oc_demo
//
//  Created by zhangxinwei on 15/12/7.
//  Copyright © 2015年 zhangxinwei. All rights reserved.
//

#ifndef ZHash_h
#define ZHash_h

#include <stdio.h>
#define var void* 

uint64_t zhashCode(const void* data, size_t size);
#endif /* ZHash_h */
