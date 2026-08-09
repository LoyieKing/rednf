#include "Method.h"

namespace nsl {

const char* IMethod::sm_szErrorMsg1 = "FileCrypt ERROR: Encryption/Decryption Object not Initialized!";
const char* IMethod::sm_szErrorMsg2 = "FileCrypt ERROR: Illegal Operation Mode!";
const char* IMethod::sm_szErrorMsg3 = "FileCrypt ERROR: Illegal Padding Mode!";
const char* IMethod::sm_szErrorMsg4 = "FileCrypt ERROR: No Key DataSpecified!";
const char* IMethod::sm_szErrorMsg5 = "FileCrypt ERROR: Key Data Length should be > 0!";
const char* IMethod::sm_szErrorMsg6 = "FileCrypt ERROR: Cannot open File ";
const char* IMethod::sm_szErrorMsg7 = "FileCrypt ERROR: The same File for Input and Output ";
const char* IMethod::sm_szErrorMsg8 = "FileCrypt ERROR: File ";
const char* IMethod::sm_szErrorMsg9 = " cannot be Correctly Decrypted!";
const char* IMethod::sm_szErrorMsg10 = " cannot be Correctly Decrypted!";

IMethod::IMethod()
{
    m_bInit = false;
}

IMethod::~IMethod()
{
}

void IMethod::Xor(char* buff, const char* chain)
{
    if (m_bInit != true)
    {
        throw CCryptorException("Xor", 0x22, sm_szErrorMsg1);
    }
    for (int i = 0; i < m_blockSize; i++)
    {
        *buff = *buff ^ *chain;
        buff = buff + 1;
        chain = chain + 1;
    }
}

void IMethod::SetMode(int iMode)
{
    if (m_bInit != true)
    {
        throw CCryptorException("SetMode", 0x2b, sm_szErrorMsg1);
    }
    if (iMode < 0 || iMode >= 3)
    {
        throw CCryptorException("SetMode", 0x2d, sm_szErrorMsg2);
    }
    m_iMode = iMode;
}

void IMethod::SetPadding(int iPadding)
{
    if (m_bInit != true)
    {
        throw CCryptorException("SetPadding", 0x35, sm_szErrorMsg1);
    }
    if (iPadding < 0 || iPadding >= 3)
    {
        throw CCryptorException("SetPadding", 0x37, sm_szErrorMsg3);
    }
    m_iPadding = iPadding;
}

int IMethod::GetKeyLength()
{
    if (m_bInit != true)
    {
        throw CCryptorException("GetKeyLength", 0x3f, sm_szErrorMsg1);
    }
    return m_keylength;
}

int IMethod::GetBlockSize()
{
    if (m_bInit != true)
    {
        throw CCryptorException("GetBlockSize", 0x46, sm_szErrorMsg1);
    }
    return m_blockSize;
}

int IMethod::GetMode()
{
    if (m_bInit != true)
    {
        throw CCryptorException("GetMode", 0x4d, sm_szErrorMsg1);
    }
    return m_iMode;
}

int IMethod::GetPadding()
{
    if (m_bInit != true)
    {
        throw CCryptorException("GetPadding", 0x54, sm_szErrorMsg1);
    }
    return m_iPadding;
}

int IMethod::Pad(char* in, int iLength)
{
    if (m_bInit != true)
    {
        throw CCryptorException("Pad", 0x5c, sm_szErrorMsg1);
    }
    int iRes = iLength % m_blockSize;
    if (iRes != 0)
    {
        int iPadded = m_blockSize - iRes;
        char* pin = in + iLength;
        switch (m_iPadding)
        {
        case 0:
            // ORIG：while 形，体内 i++ 先于 pin++。
            for (int i = 0; i < iPadded; )
            {
                *pin = '\0';
                i++;
                pin = pin + 1;
            }
            break;
        case 1:
            for (int i = 0; i < iPadded; )
            {
                *pin = ' ';
                i++;
                pin = pin + 1;
            }
            break;
        case 2:
            for (int i = 0; i < iPadded; )
            {
                *pin = (char)iPadded;
                i++;
                pin = pin + 1;
            }
            break;
        }
        return iLength + iPadded;
    }
    return iLength;
}

void IMethod::BytesToWord(const unsigned char* pucBytes, unsigned int& ruiWord)
{
    ruiWord = 0;
    ruiWord = ruiWord | ((unsigned int)*pucBytes << 0x18);
    pucBytes = pucBytes + 1;
    ruiWord = ruiWord | ((unsigned int)*pucBytes << 0x10);
    pucBytes = pucBytes + 1;
    ruiWord = ruiWord | ((unsigned int)*pucBytes << 8);
    pucBytes = pucBytes + 1;
    ruiWord = ruiWord | (unsigned int)*pucBytes;
}

void IMethod::WordToBytes(unsigned int uiWord, unsigned char* pucBytes)
{
    *pucBytes = (unsigned char)(uiWord >> 0x18);
    pucBytes = pucBytes + 1;
    *pucBytes = (unsigned char)(uiWord >> 0x10);
    pucBytes = pucBytes + 1;
    *pucBytes = (unsigned char)(uiWord >> 8);
    pucBytes = pucBytes + 1;
    *pucBytes = (unsigned char)uiWord;
}

} // namespace nsl
