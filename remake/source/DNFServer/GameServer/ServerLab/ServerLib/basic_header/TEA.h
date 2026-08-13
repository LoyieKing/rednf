#ifndef NSL_TEA_H_
#define NSL_TEA_H_

#include "Method.h"

namespace nsl {

class CTEA : public IMethod
{
public:
    CTEA();
    virtual ~CTEA();
    void Initialize(const char* keydata, int keydatalength, const char* chain, int iMode, int iPadding);

    virtual void Signature(char* szSignature);
    virtual int Encrypt(const char* szIn, char* szOut, size_t iLength);
    virtual int Decrypt(const char* szIn, char* szOut, size_t iLength);
    virtual void ResetChain();

    static const char sm_chain0[8];

private:
    void EncryptBlock(const unsigned char* pucIn, unsigned char* pucOut);
    void DecryptBlock(const unsigned char* pucIn, unsigned char* pucOut);

    char m_apKey[16];
    unsigned int m_auiKey[4];
    char m_apchain0[8];
    char m_apchain[8];
};

} // namespace nsl

#endif // NSL_TEA_H_
