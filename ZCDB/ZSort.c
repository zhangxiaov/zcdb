//
//  ZSort.c
//  ZCDB
//
//  Created by zhangxinwei on 16/2/3.
//  Copyright © 2016年 张新伟. All rights reserved.
//

#include "ZSort.h"

int  cscmpForStringAsString(char* s1, char* s2);
int cscmpForStringAsNumber(char* s1, char* s2);

//quicksort for string as num
void zsortForStringPairAsNumber(void* id, void* data, int left, int right) {
    if (left < right) {
        int i = left;
        int j = right;
        
        char key[12];
        memcpy(key, data+left*12, 12);
        
        ulong idKey = *((ulong*)id+left);
        
        while (i < j) {
            while (i < j && cscmpForStringAsNumber(key, (char*)data+j*12) != 1) {
                --j;
            }
            memcpy(data+i*12, data+j*12, 12);
            memcpy(id+i*8, id+j*8, 8);
            
            
            while (i < j && cscmpForStringAsNumber(key, (char*)data+i*12) != -1)  {
                ++i;
            }
            memcpy(data+j*12, data+i*12, 12);
            memcpy(id+j*8, id+i*8, 8);
        }
        
        strcpy(data+i*12, key);
        memcpy(id+i*8, &idKey, 8);
        
        zsortForStringPairAsNumber(id, data, left, i-1);
        zsortForStringPairAsNumber(id, data, i+1, right);
    }
}

void zsortForStringPairAsString(ulong* id, void* data, int left, int right) {
    if (left < right) {
        int i = left;
        int j = right;
        
        char key[12];
        memcpy(key, data+left*12, 12);
        
        ulong idKey = *((ulong*)id+left);
        
        while (i < j) {
            while (i < j && cscmpForStringAsString(key, (char*)data+j*12) != 1) {
                --j;
            }
            memcpy(data+i*12, data+j*12, 12);
            memcpy(id+i*8, id+j*8, 8);
            
            
            while (i < j && cscmpForStringAsString(key, (char*)data+i*12) != -1)  {
                ++i;
            }
            memcpy(data+j*12, data+i*12, 12);
            memcpy(id+j*8, id+i*8, 8);
        }
        
        strcpy(data+i*12, key);
        memcpy(id+i*8, &idKey, 8);
        
        zsortForStringPairAsString(id, data, left, i-1);
        zsortForStringPairAsString(id, data, i+1, right);
}

//quicksort for string as num
void zsortForStringAsNumber(void* data, int left, int right) {
    if (left < right) {
        int i = left;
        int j = right;
        
        char key[12];
        memcpy(key, data+left*12, 12);
        
        
        while (i < j) {
            while (i < j && cscmpForStringAsNumber(key, (char*)data+j*12) != 1) {
                --j;
            }
            memcpy(data+i*12, data+j*12, 12);
            
            while (i < j && cscmpForStringAsNumber(key, (char*)data+i*12) != -1)  {
                ++i;
            }
            memcpy(data+j*12, data+i*12, 12);
            
        }
        
        strcpy(data+i*12, key);
        zsortForStringAsNumber(data, left, i-1);
        zsortForStringAsNumber(data, i+1, right);
    }
}

//quicksort for string as string
void zsortForStringAsString(void* data, int left, int right) {
    if (left < right) {
        int i = left;
        int j = right;
        
        char key[12];
        memcpy(key, data+left*12, 12);
        
        
        while (i < j) {
            while (i < j && cscmpForStringAsString(key, (char*)data+j*12) != 1) {
                --j;
            }
            memcpy(data+i*12, data+j*12, 12);
            
            while (i < j && cscmpForStringAsString(key, (char*)data+i*12) != -1)  {
                ++i;
            }
            memcpy(data+j*12, data+i*12, 12);
            
        }
        
        strcpy(data+i*12, key);
        zsortForStringAsString(data, left, i-1);
        zsortForStringAsString(data, i+1, right);
    }
}

int  cscmpForStringAsString(char* s1, char* s2) {
    if (s1 == NULL || s2 == NULL) {
        exit(1);
    }
    
    size_t s1Len = strlen(s1);
    size_t s2Len = strlen(s2);
    
    if (s1Len < s2Len) {
        for (int i = 0; i < s1Len; i++) {
            char c1 = (char)*(s1+i);
            char c2 = (char)*(s2+i);
            if (c1 < c2) {
                return -1;
            }else if(c1 > c2) {
                return 1;
            }else {
                continue;
            }
        }
        return -1;
    }else if (s1Len > s2Len) {
        for (int i = 0; i < s2Len; i++) {
            char c1 = (char)*(s1+i);
            char c2 = (char)*(s2+i);
            if (c1 < c2) {
                return -1;
            }else if(c1 > c2) {
                return 1;
            }else {
                continue;
            }
        }
        return 1;
    }else {
        for (int i = 0; i < s1Len; i++) {
            char c1 = (char)*(s1+i);
            char c2 = (char)*(s2+i);
            if (c1 < c2) {
                return -1;
            }else if(c1 > c2) {
                return 1;
            }else {
                continue;
            }
        }
        return 0;
    }
}

int cscmpForStringAsNumber(char* s1, char* s2) {
    if (s1 == NULL || s2 == NULL) {
        exit(1);
    }
    
    size_t s1Len = strlen(s1);
    size_t s2Len = strlen(s2);
    
    if (s1Len < s2Len) {
        return -1;
    }else if (s1Len > s2Len) {
        return 1;
    }else {
        for (int i = 0; i < s1Len; i++) {
            char c1 = (char)*(s1+i);
            char c2 = (char)*(s2+i);
            if (c1 < c2) {
                return -1;
            }else if(c1 > c2) {
                return 1;
            }else {
                continue;
            }
        }
        return 0;
    }
}