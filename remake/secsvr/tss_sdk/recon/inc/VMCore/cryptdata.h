// Auto-generated header stub from DWARF info
// Original path: inc/VMCore/cryptdata.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_VMCORE_CRYPTDATA_H_H_
#define SECSVR_INC_VMCORE_CRYPTDATA_H_H_

#include "inc/common/Types.h"
#include "TenCrypt.h"

// sizeof = 4 -- ORIG DWARF (cryptdata.cpp CU) has exactly one member, the
// implicit <vptr.CCryptData> at +0; the class is polymorphic (virtual
// ~CCryptData) and carries no data member.  The old explicit `void **_vptr;`
// added a second word and made the class 8 bytes.
struct CCryptData { // line 20
public:
CCryptData(const CCryptData &arg0);
CCryptData(); // line 4
virtual ~CCryptData(); // line 9
BOOL EncryptData(short unsigned int ushCryptID, unsigned char *pbyCryptKey, bool bEnCrypt, unsigned char *pbySrcData, unsigned int unSrcDataLen); // line 19
};

#endif // SECSVR_INC_VMCORE_CRYPTDATA_H_H_
