// Auto-generated header stub from DWARF info
// Original path: inc/VMCore/ServerCryptCodeMgr.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_VMCORE_SERVERCRYPTCODEMGR_H_H_
#define SECSVR_INC_VMCORE_SERVERCRYPTCODEMGR_H_H_

#include "inc/common/Types.h"

// sizeof = 4
struct CServerCryptCodeMgr { // line 14
public:
int (***_vptr.CServerCryptCodeMgr)(...);
CServerCryptCodeMgr(const CServerCryptCodeMgr &arg0);
BOOL CallCryptCode(WORD wIndex, PBYTE pbyData, WORD wDataLen, PBYTE pbyKey, WORD wKeyLen, BOOL blIsEncode); // line 24
CServerCryptCodeMgr(); // line 14
virtual ~CServerCryptCodeMgr(); // line 19
};

#endif // SECSVR_INC_VMCORE_SERVERCRYPTCODEMGR_H_H_
