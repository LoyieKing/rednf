#include <string.h>

#include "TEA.h"
#include "SHA.h"

namespace nsl {

const char CTEA::sm_chain0[8] = {0, 0, 0, 0, 0, 0, 0, 0};

CTEA::CTEA()
{
    m_blockSize = 8;
    m_keylength = 0x10;
}

CTEA::~CTEA()
{
}

void CTEA::Initialize(const char* keydata, int keydatalength, const char* chain, int iMode, int iPadding)
{
    if (keydata == NULL)
    {
        throw CCryptorException("Initialize", 0x19, sm_szErrorMsg4);
    }
    if (keydatalength <= 0)
    {
        throw CCryptorException("Initialize", 0x1b, sm_szErrorMsg5);
    }
    if (iMode < 0 || iMode > 2)
    {
        throw CCryptorException("Initialize", 0x1d, sm_szErrorMsg2);
    }
    if (iPadding < 0 || iPadding > 2)
    {
        throw CCryptorException("Initialize", 0x1f, sm_szErrorMsg3);
    }

    m_iMode = iMode;
    m_iPadding = iPadding;

    char key[16];
    int i = 0;
    int pos = 0;
    while (i < m_keylength)
    {
        key[i] = keydata[pos];
        i = i + 1;
        pos = (pos + 1) % keydatalength;
    }

    bool bSameChain = false;
    bool bSameKey = false;
    if (m_bInit)
    {
        if (memcmp(m_apchain0, chain, m_blockSize) == 0)
        {
            bSameChain = true;
        }
        if (memcmp(m_apKey, key, m_keylength) == 0)
        {
            bSameKey = true;
        }
    }
    if (bSameChain)
    {
        memcpy(m_apchain, m_apchain0, m_blockSize);
    }
    else
    {
        memcpy(m_apchain0, chain, m_blockSize);
        memcpy(m_apchain, chain, m_blockSize);
    }
    if (!bSameKey)
    {
        memcpy(m_apKey, key, m_keylength);
        BytesToWord((const unsigned char*)key, m_auiKey[0]);
        BytesToWord((const unsigned char*)key + 4, m_auiKey[1]);
        BytesToWord((const unsigned char*)key + 8, m_auiKey[2]);
        BytesToWord((const unsigned char*)key + 12, m_auiKey[3]);
        m_bInit = true;
    }
}

void CTEA::ResetChain()
{
    if (m_bInit != true)
    {
        throw CCryptorException("ResetChain", 0x4d, sm_szErrorMsg1);
    }
    memcpy(m_apchain, m_apchain0, m_blockSize);
}

void CTEA::Signature(char* pcSig)
{
    char acSigData[23] = {0};
    strcat(acSigData, "TEA");
    int iLen = strlen(acSigData);
    memcpy(acSigData + iLen, m_apKey, m_keylength);
    sprintf(acSigData + m_keylength + iLen, "%d%d", m_iMode, m_iPadding);
    CSHA oSHA;
    oSHA.AddData(acSigData, strlen(acSigData));
    oSHA.FinalDigest(pcSig);
}

void CTEA::EncryptBlock(const unsigned char* pucIn, unsigned char* pucOut)
{
    unsigned int w;
    unsigned int v;
    BytesToWord(pucIn, v);
    BytesToWord(pucIn + 4, w);
    register unsigned int y = v;
    register unsigned int z = w;
    register unsigned int sum = 0;
    register unsigned int delta = 0x9e3779b9;
    register unsigned int n = 0x20;
    while (n-- != 0)
    {
        y += (((z << 4) ^ (z >> 5)) + z) ^ (sum + m_auiKey[sum & 3]);
        sum += delta;
        z += (((y << 4) ^ (y >> 5)) + y) ^ (sum + m_auiKey[(sum >> 0xb) & 3]);
    }
    WordToBytes(y, pucOut);
    WordToBytes(z, pucOut + 4);
}

void CTEA::DecryptBlock(const unsigned char* pucIn, unsigned char* pucOut)
{
    unsigned int w;
    unsigned int v;
    BytesToWord(pucIn, v);
    BytesToWord(pucIn + 4, w);
    register unsigned int y = v;
    register unsigned int z = w;
    register unsigned int sum = 0xc6ef3720;
    register unsigned int delta = 0x9e3779b9;
    register unsigned int n = 0x20;
    while (n-- != 0)
    {
        z -= (((y << 4) ^ (y >> 5)) + y) ^ (sum + m_auiKey[(sum >> 0xb) & 3]);
        sum -= delta;
        y -= (((z << 4) ^ (z >> 5)) + z) ^ (sum + m_auiKey[sum & 3]);
    }
    WordToBytes(y, pucOut);
    WordToBytes(z, pucOut + 4);
}

int CTEA::Encrypt(const char* in, char* result, size_t n)
{
    if (m_bInit != true)
    {
        return -1;
    }
    if (n == 0 || n % m_blockSize != 0)
    {
        return -1;
    }

    if (m_iMode == 1)
    {
        int i = 0;
        const char* pin = in;
        char* presult = result;
        for (; i < n / m_blockSize; i++)
        {
            Xor(m_apchain, pin);
            EncryptBlock((const unsigned char*)m_apchain, (unsigned char*)presult);
            memcpy(m_apchain, presult, m_blockSize);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    else if (m_iMode == 2)
    {
        int i = 0;
        const char* pin = in;
        char* presult = result;
        for (; i < n / m_blockSize; i++)
        {
            EncryptBlock((const unsigned char*)m_apchain, (unsigned char*)presult);
            Xor(presult, pin);
            memcpy(m_apchain, presult, m_blockSize);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    else
    {
        int i = 0;
        const char* pin = in;
        char* presult = result;
        for (; i < n / m_blockSize; i++)
        {
            EncryptBlock((const unsigned char*)pin, (unsigned char*)presult);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    return 1;
}

int CTEA::Decrypt(const char* in, char* result, size_t n)
{
    if (m_bInit != true)
    {
        return -1;
    }
    if (n == 0 || n % m_blockSize != 0)
    {
        return -1;
    }

    if (m_iMode == 1)
    {
        int i = 0;
        const char* pin = in;
        char* presult = result;
        for (; i < n / m_blockSize; i++)
        {
            DecryptBlock((const unsigned char*)pin, (unsigned char*)presult);
            Xor(presult, m_apchain);
            memcpy(m_apchain, pin, m_blockSize);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    else if (m_iMode == 2)
    {
        int i = 0;
        const char* pin = in;
        char* presult = result;
        for (; i < n / m_blockSize; i++)
        {
            EncryptBlock((const unsigned char*)m_apchain, (unsigned char*)presult);
            Xor(presult, pin);
            memcpy(m_apchain, pin, m_blockSize);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    else
    {
        int i = 0;
        const char* pin = in;
        char* presult = result;
        for (; i < n / m_blockSize; i++)
        {
            DecryptBlock((const unsigned char*)pin, (unsigned char*)presult);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    return 1;
}

} // namespace nsl
