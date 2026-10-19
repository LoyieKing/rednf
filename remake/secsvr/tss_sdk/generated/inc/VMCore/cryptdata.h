// Auto-generated header stub from DWARF info
// Original path: inc/VMCore/cryptdata.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_VMCORE_CRYPTDATA_H_H_
#define SECSVR_INC_VMCORE_CRYPTDATA_H_H_

#include "inc/common/Types.h"

// sizeof = 4
struct CCryptData { // line 20
public:
int (***_vptr.CCryptData)(...);
CCryptData(const CCryptData &arg0);
CCryptData(); // line 4
virtual ~CCryptData(); // line 9
BOOL EncryptData(short unsigned int ushCryptID, unsigned char *pbyCryptKey, bool bEnCrypt, unsigned char *pbySrcData, unsigned int unSrcDataLen); // line 19
};

#endif // SECSVR_INC_VMCORE_CRYPTDATA_H_H_
