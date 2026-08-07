#ifndef RIJNDAEL_H_
#define RIJNDAEL_H_

#include "stdafx.h"
#include "Method.h"
#include "SHA.h"

class CRijndael : public IMethod
{
public:
    CRijndael();
    virtual ~CRijndael();

    void Initialize(const char* keydata, int keydatalength, const char* chain, int keylength, int blockSize, int iMode, int iPadding);
    virtual int Encrypt(const char* in, char* result, size_t n);
    virtual int Decrypt(const char* in, char* result, size_t n);
    virtual void ResetChain();
    virtual void Signature(char* pcSig);

    static const char sm_chain0[32];

private:
    void DefEncryptBlock(const char* in, char* result);
    void DefDecryptBlock(const char* in, char* result);
    void EncryptBlock(const char* in, char* result);
    void DecryptBlock(const char* in, char* result);
    int Mul(int a, int b);
    int Mul4(int x);

    char m_acKey[32];
    int m_Ke[14][8];
    int m_Kd[15][8];
    int m_iROUNDS;
    char m_chain0[32];
    char m_chain[32];
    int tk[8];
    int a[8];
    int t[8];

    static const int sm_alog[256];
    static const int sm_log[256];
    static const unsigned char sm_S[256];
    static const unsigned char sm_Si[256];
    static const int sm_T1[256];
    static const int sm_T2[256];
    static const int sm_T3[256];
    static const int sm_T4[256];
    static const int sm_T5[256];
    static const int sm_T6[256];
    static const int sm_T7[256];
    static const int sm_T8[256];
    static const int sm_U1[256];
    static const int sm_U2[256];
    static const int sm_U3[256];
    static const int sm_U4[256];
    static const unsigned char sm_rcon[30];
    static const int sm_shifts[3][4][2];
};

#endif // RIJNDAEL_H_
