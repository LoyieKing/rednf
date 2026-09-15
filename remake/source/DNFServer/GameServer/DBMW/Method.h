#ifndef DBMW_METHOD_H_
#define DBMW_METHOD_H_

#include "DBMWCommon.h"


class IMethod
{
public:
    IMethod();
    virtual ~IMethod();
    // ORIG vtable 槽序（_ZTV7IMethod = 0x20）：[D1,D0,Signature,Encrypt,Decrypt,ResetChain]
    // —— 这 4 个在 ORIG 是 IMethod 的纯虚函数、由 CTEA 覆写（_ZTV4CTEA 同序）
    virtual int Signature(char* digest) = 0;
    virtual int Encrypt(char const* src, char* dst, unsigned int len) = 0;
    virtual int Decrypt(char const* src, char* dst, unsigned int len) = 0;
    virtual int ResetChain() = 0;
    int Xor(char* a, char const* b);
    int SetMode(int mode);
    int SetPadding(int padding);
    int GetKeyLength(unsigned int* out);
    int GetBlockSize(unsigned int* out);
    int GetMode(unsigned int* out);
    int GetPadding(unsigned int* out);
    int Pad(char* data, int len, unsigned int* out);
    void BytesToWord(unsigned char const* src, unsigned int& dst);
    void WordToBytes(unsigned int src, unsigned char* dst);
    char m_initialized;   // +4
    char m_pad5[3];
    int m_blockSize;      // +8
    int m_keyLength;      // +0xc
    int m_mode;           // +0x10
    int m_padding;        // +0x14
};

#endif  // DBMW_METHOD_H_
