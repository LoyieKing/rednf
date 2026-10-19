// Auto-generated header stub from DWARF info
// Original path: inc/common/VMDefines.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_COMMON_VMDEFINES_H_H_
#define SECSVR_INC_COMMON_VMDEFINES_H_H_

#include "inc/common/Types.h"

// sizeof = 8
struct tagKEYLINKNODE { // line 311
public:
tagKEYLINKNODE *next;
UINT32 key;
};
typedef tagKEYLINKNODE *PKEYLINKNODE;
// sizeof = 12
struct tagKEYTREENODE { // line 317
public:
tagKEYTREENODE *left;
tagKEYTREENODE *right;
UINT32 key;
};
typedef tagKEYTREENODE *PKEYTREENODE;
typedef UINT32 **PKEYMATRIX;
// sizeof = 24
struct tagRESERVEREGSTERS { // line 53
public:
PBYTE CBR;
PBYTE DP;
PVOID KMP;
PVOID KLP;
PVOID KTP;
UINT32 CL;
};
typedef tagRESERVEREGSTERS RESERVEREGSTERS;
// sizeof = 6
struct tagDATAHEADER { // line 74
public:
WORD Len;
WORD CodeLen;
WORD DataLen;
};
typedef tagDATAHEADER DATAHEADER;

#endif // SECSVR_INC_COMMON_VMDEFINES_H_H_
