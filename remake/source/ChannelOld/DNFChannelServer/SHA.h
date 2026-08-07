#ifndef SHA_H_
#define SHA_H_

#include "stdafx.h"
#include "Method.h"

class CSHA
{
public:
    CSHA();
    void AddData(const char* pcData, int iDataLength);
    void FinalDigest(char* pcDigest);
    void Reset();

private:
    void Transform();

    static uint CH(uint x, uint y, uint z)
    {
        return (z ^ y) & x ^ z;
    }
    static uint MAJ(uint x, uint y, uint z)
    {
        return (y | x) & z | y & x;
    }
    static uint SIG0(uint x)
    {
        return (x >> 0x16 | x << 10) ^ (x >> 2 | x << 0x1e) ^ (x >> 0xd | x << 0x13);
    }
    static uint SIG1(uint x)
    {
        return (x >> 0x19 | x << 7) ^ (x >> 6 | x << 0x1a) ^ (x >> 0xb | x << 0x15);
    }
    static uint sig0(uint x)
    {
        return (x >> 7 | x << 0x19) ^ (x >> 0x12 | x << 0xe) ^ (x >> 3);
    }
    static uint sig1(uint x)
    {
        return (x >> 0x11 | x << 0xf) ^ (x >> 0x13 | x << 0xd) ^ (x >> 10);
    }
    static void Bytes2Word(const unsigned char* pcBytes, unsigned int& ruiWord);
    static void Word2Bytes(const unsigned int& ruiWord, unsigned char* pcBytes);

    uint m_auiBuf[8];
    uint m_auiBits[2];
    unsigned char m_aucIn[64];
    bool m_bAddData;

    static const uint sm_K256[64];
    static const uint sm_H256[8];
};

#endif // SHA_H_
