#ifndef DBMW_SHA_H_
#define DBMW_SHA_H_

#include "DBMWCommon.h"


class CSHA
{
public:
    CSHA();
    int AddData(char const* data, int len);
    void Reset();
    int FinalDigest(char* digest);
    void Transform();
    unsigned int CH(unsigned int x, unsigned int y, unsigned int z);
    unsigned int MAJ(unsigned int x, unsigned int y, unsigned int z);
    unsigned int SIG0(unsigned int x);
    unsigned int SIG1(unsigned int x);
    unsigned int sig0(unsigned int x);
    unsigned int sig1(unsigned int x);
    void Bytes2Word(unsigned char const* src, unsigned int& dst);
    void Word2Bytes(unsigned int const& src, unsigned char* dst);
    unsigned int m_H[8];        // +0
    unsigned int m_lengthLo;    // +0x20
    unsigned int m_lengthHi;    // +0x24
    unsigned char m_block[0x40]; // +0x28
    unsigned char m_finalized;  // +0x68
    static const unsigned int sm_K256[64];
    static const unsigned int sm_H256[8];
};

#endif  // DBMW_SHA_H_
