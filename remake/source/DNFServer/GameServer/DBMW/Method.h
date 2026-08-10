#ifndef DBMW_METHOD_H_
#define DBMW_METHOD_H_

#include "DBMWCommon.h"


class IMethod
{
public:
    IMethod();
    virtual ~IMethod();
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
