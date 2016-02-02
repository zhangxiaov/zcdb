//
//  ZJson.c
//  oc_demo
//
//  Created by zhangxinwei on 15/12/7.
//  Copyright © 2015年 zhangxinwei. All rights reserved.
//

#include "ZJson.h"
#include <stdarg.h>
#include <sys/malloc.h>
#include <string.h>
#include "ZTime.h"
#include "CString.h"
#include "ZStack.h"
#include "ZMap.h"
#include "ZArray.h"

struct _CZ {
    int type;
    int typeSize;
};

struct _KV {
    char* key;
    void* val;
    KV* next;
};

typedef struct _zjsonStr zjsonStr;

struct _zjsonStr {
    char* str;
    int nextTokenIndex;
};

int hasStatus(int self, int expectedStatus);
int zjsonStrNextType(zjsonStr* jsonStr);
void* zjsonStrReadNumber(zjsonStr* jsonStr);
void* zjsonStrReadBoolean(zjsonStr* jsonStr);
void zjsonStrReadNull(zjsonStr* jsonStr);
char* zjsonStrReadString(zjsonStr* self);

typedef struct _zjsonNode zjsonNode;

struct _zjsonNode {
    char* key;
    void* val;
    zjsonNode* next;
};

ZJson* zjsonInitWithString(char* jsonStr) {
    ZJson* json = (ZJson*)malloc(sizeof(ZJson));
    json->type = ZJSON;
    json->data = (zjsonNode*)malloc(sizeof(zjsonNode));
    json->originString = jsonStr;
    char c = (char)(*jsonStr);
    
    zjsonStr* zjstr = (zjsonStr*)malloc(sizeof(zjsonStr));
    zjstr->str = jsonStr;
    zjstr->nextTokenIndex = 0;
    
    int i = 0;
    size_t len = strlen(jsonStr);
    
    ZStack* stack = zstackInit(10, typePtr);
    
    int status = STATUS_EXPECT_SINGLE_VALUE | STATUS_EXPECT_BEGIN_OBJECT | STATUS_EXPECT_BEGIN_ARRAY;
    for (; ;) {
        int type = zjsonStrNextType(zjstr);
        switch (type) {
            case idnumber:
                if (hasStatus(status, STATUS_EXPECT_SINGLE_VALUE)) {//single number
                    zjsonNumber number = zjsonStrReadNumber(zjstr);
                    zstackPush(stack, number);
                    status = STATUS_EXPECT_END_DOCUMENT;
                    continue;
                }
                if (hasStatus(status, STATUS_EXPECT_OBJECT_VALUE)) {
                    zjsonNumber number = zjsonStrReadNumber(zjstr);
                    char* key = zstackPop(stack);
                    zmapPut(zstackPeek(stack), key, number);
                    status = STATUS_EXPECT_COMMA | STATUS_EXPECT_END_OBJECT;
                    continue;
                }
                if (hasStatus(status, STATUS_EXPECT_ARRAY_VALUE)) {
                    zjsonNumber number = zjsonStrReadNumber(zjstr);
                    zarrayAdd(zstackPeek(stack), number);
                    status = STATUS_EXPECT_COMMA | STATUS_EXPECT_END_ARRAY;
                    continue;
                }
                printf("Unexpected umber\n");
                break;
            case idbool:
                if (hasStatus(status, STATUS_EXPECT_SINGLE_VALUE)) {// single boolean:
                    zjsonBoolean boolean = zjsonStrReadBoolean(zjstr);
                    zstackPush(stack, boolean);
                    status = STATUS_EXPECT_END_DOCUMENT;
                    continue;
                }
                if (hasStatus(status, STATUS_EXPECT_OBJECT_VALUE)) {
                    zjsonBoolean boolean = zjsonStrReadBoolean(zjstr);
                    
                    char* key = zstackPop(stack);
                    zmapPut(zstackPeek(stack), key, boolean);

                    status = STATUS_EXPECT_COMMA | STATUS_EXPECT_END_OBJECT;
                    continue;
                }
                if (hasStatus(status, STATUS_EXPECT_ARRAY_VALUE)) {
                    zjsonBoolean boolean = zjsonStrReadBoolean(zjstr);
                    zarrayAdd(zstackPeek(stack), boolean);
                    status = STATUS_EXPECT_COMMA | STATUS_EXPECT_END_ARRAY;
                    continue;
                }
                printf("Unexpected bool\n");
                break;
            case idstring:
                if (hasStatus(status, STATUS_EXPECT_SINGLE_VALUE)) {// single string:
                    char* str = zjsonStrReadString(zjstr);
                    zstackPush(stack, str);
                    status = STATUS_EXPECT_END_DOCUMENT;
                    continue;
                }
                if (hasStatus(status, STATUS_EXPECT_OBJECT_KEY)) {
                    char* str = zjsonStrReadString(zjstr);
                    zstackPush(stack, str);
                    status = STATUS_EXPECT_COLON;
                    continue;
                }
                if (hasStatus(status, STATUS_EXPECT_OBJECT_VALUE)) {
                    char* str = zjsonStrReadString(zjstr);
                    char* key = zstackPop(stack);
                    zmapPut(zstackPeek(stack), key, str);
                    status = STATUS_EXPECT_COMMA | STATUS_EXPECT_END_OBJECT;
                    continue;
                }
                if (hasStatus(status, STATUS_EXPECT_ARRAY_VALUE)) {
                    char* str = zjsonStrReadString(zjstr);
                    zarrayAdd(zstackPeek(stack), str);
                    status = STATUS_EXPECT_COMMA | STATUS_EXPECT_END_ARRAY;
                    continue;
                }
                printf("Unexpected string \n");
                break;
            case idnull:
                if (hasStatus(status, STATUS_EXPECT_SINGLE_VALUE)) {// single null:
                    zjsonStrReadNull(zjstr);
                    zstackPush(stack, NULL);
                    status = STATUS_EXPECT_END_DOCUMENT;
                    continue;
                }
                if (hasStatus(status, STATUS_EXPECT_OBJECT_VALUE)) {
                    zjsonStrReadNull(zjstr);
                    char* key = zstackPop(stack);
                    zmapPut(zstackPeek(stack), key, NULL);
                    status = STATUS_EXPECT_COMMA | STATUS_EXPECT_END_OBJECT;
                    continue;
                }
                if (hasStatus(status, STATUS_EXPECT_ARRAY_VALUE)) {
                    zjsonStrReadNull(zjstr);
                    zarrayAdd(zstackPeek(stack), NULL);
                    status = STATUS_EXPECT_COMMA | STATUS_EXPECT_END_ARRAY;
                    continue;
                }
                printf("Unexpected null\n");
                break;
            case idobjbegin:// {
                if (hasStatus(status, STATUS_EXPECT_BEGIN_OBJECT)) {
                    zstackPush(stack, zmapInit());
                    status = STATUS_EXPECT_OBJECT_KEY | STATUS_EXPECT_BEGIN_OBJECT | STATUS_EXPECT_END_OBJECT;
                    continue;
                }
                printf("Unexpected '{'\n");
                break;
            case idobjend:// }
                if (hasStatus(status, STATUS_EXPECT_END_OBJECT)) {
                    ZMap* map = zstackPop(stack);
                    if (zstackEmpty(stack)) {//root object
                        zstackPush(stack, map);
                        status = STATUS_EXPECT_END_DOCUMENT;
                        continue;
                    }
                    
                    int type = zstackPreValueType(stack);
                    if (type == typeZArray) {
                        zarrayAdd(zstackPeek(stack), map);
                        status = STATUS_EXPECT_COMMA | STATUS_EXPECT_END_ARRAY;
                        continue;
                    }else {
                        char* key = zstackPop(stack);
                        zmapPut(zstackPeek(stack), key, map);
                        status = STATUS_EXPECT_COMMA | STATUS_EXPECT_END_OBJECT;
                        continue;
                    }
//                    if (type == TYPE_OBJECT_KEY) {
//                        char* key = zstackPop(stack);
//                        zmapPut(zstackPeek(stack), key, map);
//                        status = STATUS_EXPECT_COMMA | STATUS_EXPECT_END_OBJECT;
//                        continue;
//                    }
                }
                printf("Unexpected '}'\n");
                break;
            case idlistbegin:// [
                if (hasStatus(status, STATUS_EXPECT_BEGIN_ARRAY)) {
                    zstackPush(stack, zarrayInit(true));
                    status = STATUS_EXPECT_ARRAY_VALUE | STATUS_EXPECT_BEGIN_OBJECT | STATUS_EXPECT_BEGIN_ARRAY | STATUS_EXPECT_END_ARRAY;
                    continue;
                }
                printf("Unexpected '['\n");
                break;
            case idlistend: // ]
                if (hasStatus(status, STATUS_EXPECT_END_ARRAY)) {
                    ZArray* array = zstackPop(stack);
                    if (zstackEmpty(stack)) {
                        zstackPush(stack, array);
                        status = STATUS_EXPECT_END_DOCUMENT;
                        continue;
                    }

                    int type = zstackPreValueType(stack);
                    if (type == typeZArray) {// xx, xx, [CURRENT] ,]
                        zarrayAdd(zstackPeek(stack), array);
                        status = STATUS_EXPECT_COMMA | STATUS_EXPECT_END_ARRAY;
                        continue;
                    }
                    
                    // 没有类型字段的先默认是字符串的
                    if (type > typeZStruct || type < typeZStack) {// key: [ CURRENT ] ,}
                        char* key = zstackPop(stack);
                        zmapPut(zstackPeek(stack), key, array);
                        status = STATUS_EXPECT_COMMA | STATUS_EXPECT_END_OBJECT;
                        continue;
                    }
                }
                printf("Unexpected char ']'\n");
                break;
                
            case idcolon://:
                if (status == STATUS_EXPECT_COLON) {
                    status = STATUS_EXPECT_OBJECT_VALUE | STATUS_EXPECT_BEGIN_OBJECT | STATUS_EXPECT_BEGIN_ARRAY;
                    continue;
                }
                printf("Unexpected colon char\n");
                break;
            case idcomma://,
                if (hasStatus(status, STATUS_EXPECT_COMMA)) {
                    if (hasStatus(status, STATUS_EXPECT_END_OBJECT)) {
                        status = STATUS_EXPECT_OBJECT_KEY;
                        continue;
                    }
                    if (hasStatus(status, STATUS_EXPECT_END_ARRAY)) {
                        status = STATUS_EXPECT_ARRAY_VALUE | STATUS_EXPECT_BEGIN_ARRAY | STATUS_EXPECT_BEGIN_OBJECT;
                        continue;
                    }
                }
                printf("Unexpected comma char\n");
                break;
            case idend:
                if (hasStatus(status, STATUS_EXPECT_END_DOCUMENT)) {//STATUS_EXPECT_END_OBJECT
                    void* jsonData = zstackPop(stack);
                    if (zstackEmpty(stack)) {
                        json->data = jsonData;
                        return json;
                    }
                }
                printf("Unexpected EOF \n");
                return NULL;
            default:
                break;
        }
    }
    
    return json;
}

int hasStatus(int self, int expectedStatus) {
    return ((self & expectedStatus) > 0);
}

char zjsonStrReadNext(zjsonStr* jsonStr) {
    char ch = jsonStr->str[jsonStr->nextTokenIndex];
    jsonStr->nextTokenIndex++;
    return ch;
}

char zjsonStrReadPeek(zjsonStr* jsonStr) {
    
    return jsonStr->str[jsonStr->nextTokenIndex];
}

int zjsonStrNextType(zjsonStr* jsonStr) {
    char ch = '?';
    for (;;) {
        if (jsonStr->nextTokenIndex > strlen(jsonStr->str) - 1) {
            return idend;
        }
        ch = zjsonStrReadPeek(jsonStr);
        if (!isWhiteSpace(ch)) {
            break;
        }
        zjsonStrReadNext(jsonStr); // skip white space
    }
    switch (ch) {
        case '{':
            zjsonStrReadNext(jsonStr); // skip white space
            return idobjbegin;
        case '}':
            zjsonStrReadNext(jsonStr); // skip white space
            return idobjend;
        case '[':
            zjsonStrReadNext(jsonStr); // skip white space
            return idlistbegin;
        case ']':
            zjsonStrReadNext(jsonStr); // skip white space
            return idlistend;
        case ':':
            zjsonStrReadNext(jsonStr); // skip white space
            return idcolon;
        case ',':
            zjsonStrReadNext(jsonStr); // skip white space
            return idcomma;
        case '\"':
            return idstring;
        case 'n':
            return idnull;
            // true / false
        case 't':
        case 'f':
            return idbool;
        case '-':
            return idnumber;
    }
    if (ch >= '0' && ch <= '9') {
        return idnumber;
    }
    printf("Parse error when try to guess next token.\n");
    return 11;
}

#define READ_NUMBER_INT_PART 1
#define READ_NUMBER_FRA_PART 2
#define READ_NUMBER_EXP_PART 3
#define READ_NUMBER_END 4


void* zjsonStrReadNumber(zjsonStr* jsonStr) {
    
    char* s = "123456";
//    char* intPart = ""; // ###.xxxExxx
//    char* fraPart = ""; // xxx.###Exxx
//    char* expPart = ""; // xxx.xxxE###
//
//    bool hasFraPart = false;
//    bool hasExpPart = false;
//    
//    bool expMinusSign = false;
//    
//    char ch = zjsonStrReadPeek(jsonStr);
//
//
//    if (1) {
//        zjsonStrReadNext(jsonStr);
//    }
//    int status = READ_NUMBER_INT_PART;
//    for (;;) {
//        if (jsonStr->nextTokenIndex < strlen(jsonStr->str)) {
//            ch = zjsonStrReadPeek(jsonStr);
//        } else {
//            status = READ_NUMBER_END;
//        }
//        switch (status) {
//            case READ_NUMBER_INT_PART:
//                if (ch >= '0' && ch <= '9') {
//                    intPart = csAppendChar(intPart, zjsonStrReadNext(jsonStr));
//                } else if (ch == '.') {
//                    if (strlen(intPart) == 0) {
//                        printf("Unexpected char\n");
//                    }
//                    zjsonStrReadNext(jsonStr);
//                    hasFraPart = true;
//                    status = READ_NUMBER_FRA_PART;
//                } else if (ch == 'e' || ch == 'E') {
//                    zjsonStrReadNext(jsonStr);
//                    hasExpPart = true; // try to determin exp part's sign:
//                    char signChar = zjsonStrReadPeek(jsonStr);
//                    if (signChar == '-' || signChar == '+') {
//                        expMinusSign = signChar == '-';
//                        zjsonStrReadNext(jsonStr);
//                    }
//                    status = READ_NUMBER_EXP_PART;
//                } else {
//                    if (strlen(intPart) ==  0) {
//                        printf("Unexpected char\n");
//                        return NULL;
//                    }
//                    // end of number:
//                    status = READ_NUMBER_END;
//                }
//                continue;
//            case READ_NUMBER_FRA_PART:
//                if (ch >= '0' && ch <= '9') {
//                    fraPart = csAppendChar(fraPart, zjsonStrReadNext(jsonStr));
//                } else if (ch == 'e' || ch == 'E') {
//                    zjsonStrReadNext(jsonStr);
//                    hasExpPart = true;             // try to determin exp part's sign:
//                    char signChar = zjsonStrReadPeek(jsonStr);
//                    if (signChar == '-' || signChar == '+') {
//                        expMinusSign = signChar == '-';
//                        zjsonStrReadNext(jsonStr);
//                    }
//                    status = READ_NUMBER_EXP_PART;
//                } else {
//                    if (strlen(fraPart) == 0) {
//                        printf("Unexpected char\n");
//                        return NULL;
//                    }
//                    // end of number:
//                    status = READ_NUMBER_END;
//                }
//                continue;
//            case READ_NUMBER_EXP_PART:
//                if (ch >= '0' && ch <= '9') {
//                    expPart = csAppendChar(expPart, zjsonStrReadNext(jsonStr));
//                } else {
//                    if (strlen(expPart) == 0) {
//                        printf("Unexpected char\n");
//                        return NULL;
//                    }
//                    // end of number:
//                    status = READ_NUMBER_END;
//                }
//                continue;
//            case READ_NUMBER_END:
//                // build parsed number:
////                int readed = reader.readed;
//                return intPart;
//        }
//        continue;
//    }
    return NULL;
}

void* zjsonStrReadBoolean(zjsonStr* jsonStr) {
    char ch = zjsonStrReadNext(jsonStr);
    char* expected = NULL;
    if (ch == 't') {
        expected = "rue"; // true
        for (int i = 0; i <strlen(expected); i++) {
            char theChar = zjsonStrReadNext(jsonStr);
            if (theChar != expected[i]) {
                printf("Unexpected char \n");
                return NULL;
            }
        }
        return "true";
    } else if (ch == 'f') {
        expected = "alse"; // false
        for (int i = 0; i <strlen(expected); i++) {
            char theChar = zjsonStrReadNext(jsonStr);
            if (theChar != expected[i]) {
                printf("Unexpected char \n");
                return NULL;
            }
        }
        return "false";
    } else {
        printf("Unexpected char\n");
        return NULL;
    }
}

void zjsonStrReadNull(zjsonStr* jsonStr) {
    char* expected = "null";
    for (int i = 0; i < strlen(expected); i++) {
        char theChar = zjsonStrReadNext(jsonStr);
        if (theChar != expected[i]) {
            printf("Unexpected char\n");
            return;
        }
    }
}

char* zjsonStrReadString(zjsonStr* self) {
    char* str = "";
    // first char must be "
    char ch = zjsonStrReadNext(self);
    if (ch != '\"') {
        printf("Unexpected char \"\n");
        return NULL;
    }
    for (;;) {
        ch = zjsonStrReadNext(self);
        if (ch == '\\') { // escape: \" \\ \/ \b \f \n \r \t
            char ech = zjsonStrReadNext(self);
            switch (ech) {
                case '\"':
                    str = csAppend(str, "\"");
                    break;
                case '\\':
                    str = csAppend(str, "\\");
                    break;
                case '/':
                    str = csAppend(str, "/");
                    break;
                case 'b':
                    str = csAppend(str, "\b");
                    break;
                case 'f':
                    str = csAppend(str, "\f");
                    break;
                case 'n':
                    str = csAppend(str, "\n");
                    break;
                case 'r':
                    str = csAppend(str, "\r");
                    break;
                case 't':
                    str = csAppend(str, "\t");
                    break;
                case 'u':
                    // read an unicode uXXXX:
                    ;
                    int u = 0;
                    for (int i = 0; i < 4; i++) {
                        char uch = zjsonStrReadNext(self);
                        if (uch >= '0' && uch <= '9') {
                            u = (u << 4) + (uch - '0');
                        } else if (uch >= 'a' && uch <= 'f') {
                            u = (u << 4) + (uch - 'a') + 10;
                        } else if (uch >= 'A' && uch <= 'F') {
                            u = (u << 4) + (uch - 'A') + 10;
                        } else {
                            printf("Unexpected char\n");
                        }
                    }
                    str = csAppendChar(str, u);
                    break;
                default:
                    printf("Unexpected char\n");
                    break;
            }
        } else if (ch == '\"') { // end of string:
            break;
        } else if (ch == '\r' || ch == '\n') {
            printf("Unexpected char\n");
            return NULL;
        } else {
            str = csAppendChar(str, ch);
        }
    }
    
    str = csAppendChar(str, '\0');
    
    return str;
}

char* zjsonArrayToString(ZJson* jsonArray) {
    char* str = "[";

    int type = jsonArray->type;
    int count = jsonArray->count;
    void* data = jsonArray->data;
    
    KV* kv = data;
    for (int i = 0; i < count; i++) {
        char* k = kv->key;

        // val
        CZ* cz = (CZ*)kv->val;
        char type = cz->type;
        switch (type) {
            case ZI:
                str = csAppendInt(str, ((zi*)cz)->val);
                str = csAppend(str, ",");
                break;
            case ZJSONARRAY:
                str = csAppend(str, zjsonArrayToString(((ZJson*)cz)));
                break;
            case ZJSON:
                str = csAppend(str, zjsonToString(((ZJson*)cz)));
                break;
            case ZS:
                str = csAppend(str, "\"");
                str = csAppend(str, ((zs*)cz)->val);
                str = csAppend(str, "\",");
                break;
            default:
                break;
        }

        kv = kv->next;
    }

    str = csReplaceCharAtLast(str, ']');
    str = csAppend(str, ",");
    return str;
}

char* zjsonToString(ZJson* json) {
    char* str = "";
    ZStruct* data = json->data;
    int type = data->type;
    switch (type) {
        case typeZArray:
            str = csAppend(str, zarrayToString((ZArray*)data));
            break;
        case typeZMap:
            str = csAppend(str,  zmapToString((ZMap*)data));
            break;
        default:
            str = csAppend(str, "\"");
            str = csAppend(str, (char*)data);
            str = csAppend(str, "\"");
            break;
    }
    
    return str;
}

void main_zjson() {
//void main(){
    
    //1 obj
//    char* jsonString = "{\"arrayInnerK\":\"arrayInnerv\"}";
//    
//    ZJson* json = zjsonInitWithString(jsonString);
//    ZMap* map = json->data;
//    
//    printf("%s \n", zmapGet(map, "arrayInnerK"));
//    printf("%s \n", zmapGet(map, "age"));


    
    //2 array
//    char* arrayString = "[\"true\", \"test\",{\"arrayInnerK\":\"arrayInnerv\"}]";
//
//    ZJson* json_array = zjsonInitWithString(arrayString);
//    ZArray* array = json_array->data;
//    
//    printf("%s \n", zarrayGet(array, 0));
//    printf("%s \n", zarrayGet(array, 1));
//    
//    ZMap* m = zarrayGet(array, 2);
//    printf("%s \n",zmapGet(m, "arrayInnerK"));
//    printf("%s \n", zarrayGet(array, 3));
    
    //3
    size_t s = ztimeSince1970();
    for (int i = 0; i < 100000; i++) {
        char* jsonString = "{\"name\":\"张新伟\",\"age\":\"25\",\"job\":\"nojob\",\"array\":[\"11\",\"true\",\"test\",{\"arrayInnerK\":\"arrayInnerv\"}]}";
        
        ZJson* json = zjsonInitWithString(jsonString);
        ZMap* map = json->data;
        ZArray* array = zmapGet(map, "array");
        
//        printf("%s \n", zmapGet(map, "name"));
//        printf("%s \n", zarrayGet(array, 0));
        
        ZMap* innerM = zarrayGet(array, 3);
//        zarrayGet(array, 4);
//        printf("%s \n", zmapGet(innerM, "arrayInnerK"));
        
        //
        zmapPut(innerM, "arrayInnerK", "arrayInnervvv2");
        
//        printf("%s \n", zjsonToString(json));
        zjsonToString(json);

    }
    
    printf("time : =%llu \n", ztimeSince1970() - s);
}










