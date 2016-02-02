//
//  ZTime.c
//  oc_demo
//
//  Created by zhangxinwei on 15/12/7.
//  Copyright © 2015年 zhangxinwei. All rights reserved.
//

#include "ZTime.h"

uint64_t ztimeSince1970() {
    return time(NULL);
}