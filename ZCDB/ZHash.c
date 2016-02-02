//
//  ZHash.c
//  oc_demo
//
//  Created by zhangxinwei on 15/12/7.
//  Copyright © 2015年 zhangxinwei. All rights reserved.
//

#include "ZHash.h"
#include "ZTime.h"
#include <string.h>

uint64_t MurmurHash64A ( const void * key, int len, unsigned int seed )
{
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;
    
    uint64_t h = seed ^ (len * m);
    
    const uint64_t * data = (const uint64_t *)key;
    const uint64_t * end = data + (len/8);
    
    while(data != end)
    {
        uint64_t k = *data++;
        
        k *= m;
        k ^= k >> r;
        k *= m;
        
        h ^= k;
        h *= m;
    }
    
    const unsigned char * data2 = (const unsigned char*)data;
    
    switch(len & 7)
    {
        case 7: h ^= (uint64_t)(data2[6]) << 48;
        case 6: h ^= (uint64_t)(data2[5]) << 40;
        case 5: h ^= (uint64_t)(data2[4]) << 32;
        case 4: h ^= (uint64_t)(data2[3]) << 24;
        case 3: h ^= (uint64_t)(data2[2]) << 16;
        case 2: h ^= (uint64_t)(data2[1]) << 8;
        case 1: h ^= (uint64_t)(data2[0]);
            h *= m;
    };
    
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    
    return h;
}

uint64_t zhashCode(const void* data, size_t size) {
    
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;
    const uint64_t * d = (const uint64_t*)data;
    const uint64_t * end = d + (size/8);
    
    uint64_t h = 0xCe110 ^ (size * m);
    
    while (d != end) {
        uint64_t k = *d++;
        k *= m;
        k ^= k >> r;
        k *= m;
        h ^= k;
        h *= m;
    }
    
    const unsigned char * data2 = (const unsigned char*)d;
    
    switch (size & 7) {
        case 7: h ^= (uint64_t)(data2[6]) << 48;
        case 6: h ^= (uint64_t)(data2[5]) << 40;
        case 5: h ^= (uint64_t)(data2[4]) << 32;
        case 4: h ^= (uint64_t)(data2[3]) << 24;
        case 3: h ^= (uint64_t)(data2[2]) << 16;
        case 2: h ^= (uint64_t)(data2[1]) << 8;
        case 1: h ^= (uint64_t)(data2[0]);
            h *= m;
    };
    
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    
    return h;
    
}

void main_hash() {
    char* s = "ffafafafaffa";
    char* s2 = "key";
    char* s3 = "key2";
    char* s4 = "key1";
    char* s5 = "keyTest";

    char* s6 = "keyObj";
    
    uint64_t t = ztimeSince1970();
    for (int i = 0; i< 100000000; i++) {
//        MurmurHash64A(s, strlen(s), 0xCe110);
        zhashCode(s, strlen(s));
        
//        printf("%llu\n", );
//        printf("%llu\n", zhashCode(s2, sizeof(s)));
//        printf("%llu\n", zhashCode(s3, sizeof(s)));
//        printf("%llu\n", zhashCode(s4, sizeof(s)));
//        printf("%llu\n", zhashCode(s5, sizeof(s)));
//        printf("%llu\n", zhashCode(s6, sizeof(s)));
    }
    
    printf("need time := %llu sec\n", ztimeSince1970() - t);

}