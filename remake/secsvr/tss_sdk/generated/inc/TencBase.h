// Auto-generated header stub from DWARF info
// Original path: inc/TencBase.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_TENCBASE_H_H_
#define SECSVR_INC_TENCBASE_H_H_

#include "include/TenCrypt.h"

// sizeof = 12
struct CTsLocalImp : public ITsLocal { // line 8
private:
void *m_pvGetModuleHandle;
void *m_pvGetProcAddress;
public:
CTsLocalImp(const CTsLocalImp &arg0);
CTsLocalImp(); // line 1256
virtual ~CTsLocalImp(); // line 1269
virtual void ProcessLastBytes(unsigned char *pbyKey, bool bEncrypt, unsigned int dwBufLen, unsigned int dwKeySize, unsigned char *pbyBuf); // line 1283
virtual void * LocalGetModuleHandle(); // line 1273
virtual void * LocalGetProcAddress(); // line 1278
virtual void Seattos(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen); // line 1336
};


extern void BOX3D1(unsigned char *key, unsigned int keylen, bool bnec, unsigned char *buf); // line 172 (inferred)
extern void BOX3D2(unsigned char *key, unsigned int keylen, bool bnec, unsigned char *buf); // line 253 (inferred)
extern void BOX3D3(unsigned char *key, unsigned int keylen, bool bnec, unsigned char *buf); // line 358 (inferred)
extern void BOX3D4(unsigned char *key, unsigned int keylen, bool bnec, unsigned char *buf); // line 487 (inferred)
extern void BOX3D5(unsigned char *key, unsigned int keylen, bool bnec, unsigned char *buf); // line 643 (inferred)
extern void BOX3D6(unsigned char *key, unsigned int keylen, bool bnec, unsigned char *buf); // line 823 (inferred)
extern void BOX3D7(unsigned char *key, unsigned int keylen, bool bnec, unsigned char *buf); // line 1027 (inferred)

#endif // SECSVR_INC_TENCBASE_H_H_
