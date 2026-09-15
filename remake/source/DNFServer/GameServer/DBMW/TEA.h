#ifndef DBMW_TEA_H_
#define DBMW_TEA_H_

#include "DBMWCommon.h"


class CTEA : public IMethod
{
public:
    CTEA();
    virtual ~CTEA();
    static const char sm_chain0[8];  // 初始链向量（全零）
    int Initialize(char const* key, int keyLen, char const* iv, int ivLen, int mode);
    virtual int ResetChain();        // 覆写 IMethod 纯虚（ORIG vtable 槽 7）
    int EncryptBlock(unsigned char const* src, unsigned char* dst);
    int DecryptBlock(unsigned char const* src, unsigned char* dst);
    virtual int Encrypt(char const* src, char* dst, unsigned int len);   // 槽 5
    virtual int Decrypt(char const* src, char* dst, unsigned int len);   // 槽 6
    virtual int Signature(char* digest);                                 // 槽 4
    unsigned char m_key[0x10];  // +0x18
    unsigned char m_iv[0x10];   // +0x28（轮密钥）
    unsigned char m_chain[8];   // +0x38
    unsigned char m_chain2[8];  // +0x40
};

#endif  // DBMW_TEA_H_
