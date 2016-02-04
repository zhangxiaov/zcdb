////
////  main.c
////  ZCDB
////
////  Created by 张新伟 on 16/2/1.
////  Copyright © 2016年 张新伟. All rights reserved.
////
//
//#include <string.h>
//#include <stdio.h>
//
//typedef  unsigned long ulong;
//
//int ccscmpForStringAsNumber(char* s1, char* s2) {
//    if (s1 == NULL || s2 == NULL) {
//        exit(1);
//    }
//    
//    size_t s1Len = strlen(s1);
//    size_t s2Len = strlen(s2);
//    
//    if (s1Len < s2Len) {
//        return -1;
//    }else if (s1Len > s2Len) {
//        return 1;
//    }else {
//        for (int i = 0; i < s1Len; i++) {
//            char c1 = (char)*(s1+i);
//            char c2 = (char)*(s2+i);
//            if (c1 < c2) {
//                return -1;
//            }else if(c1 > c2) {
//                return 1;
//            }else {
//                continue;
//            }
//        }
//        return 0;
//    }
//}
//
////quicksort for string as num
//void zzsortForStringPairAsNumber(void* id, void* data, int left, int right) {
//    if (left < right) {
//        int i = left;
//        int j = right;
//        
//        char key[12];
//        memcpy(key, data+left*12, 12);
//        
//        ulong idKey = *((ulong*)id+left);
//        
//        while (i < j) {
//            while (i < j && ccscmpForStringAsNumber(key, (char*)data+j*12) != 1) {
//                --j;
//            }
//            memcpy(data+i*12, data+j*12, 12);
//            memcpy(id+i*8, id+j*8, 8);
//            
//            
//            while (i < j && ccscmpForStringAsNumber(key, (char*)data+i*12) != -1)  {
//                ++i;
//            }
//            memcpy(data+j*12, data+i*12, 12);
//            memcpy(id+j*8, id+i*8, 8);
//        }
//        
//        strcpy(data+i*12, key);
//        memcpy(id+i*8, &idKey, 8);
//        
//        zzsortForStringPairAsNumber(id, data, left, i-1);
//        zzsortForStringPairAsNumber(id, data, i+1, right);
//    }
//}
//
//int main(void)
//{
//    
////    int ii[] = {11111, 22222, 3333, 55555555, 99,0, 777777777, 77777};
//
////    quickSort(&ii, 0, 7);
//    
////    for (int i = 0; i < 8; i++) {
////        printf("%d\n", ii[i]);
////    }
//    
//    ulong ids[] = {11111, 22222, 3333, 55555555, 99,0, 777777777, 77777};
//    char ss[8][12]= {"11111", "22222", "3333", "55555555", "99", "0", "777777777", "77777"};
//    zzsortForStringPairAsNumber(&ids ,ss, 0, 7);
//    
//    for (int i = 0; i < 8; i++) {
//        printf("%s\n", ss[i]);
//    }
//    printf("====;\n");
//    for (int i = 0; i < 8; i++) {
//        printf("%ld\n", ids[i]);
//    }
//    return 0;
//}
//
