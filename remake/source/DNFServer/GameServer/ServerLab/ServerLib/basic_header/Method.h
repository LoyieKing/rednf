#ifndef NSL_METHOD_H_
#define NSL_METHOD_H_

#include <string>

namespace nsl {

class CCryptorException
{
public:
    CCryptorException(const char* strOccFunction, unsigned int occLine, const char* strMessage)
        : strOccFunction_(strOccFunction), occLine_(occLine), strMessage_(strMessage)
    {
    }
    ~CCryptorException()
    {
    }

    std::string getOccFunction() const
    {
        return strOccFunction_;
    }
    unsigned int getOccLine() const
    {
        return occLine_;
    }
    std::string getMessage() const
    {
        return strMessage_;
    }

private:
    std::string strOccFunction_;
    unsigned int occLine_;
    std::string strMessage_;
};

class IMethod
{
public:
    IMethod();
    virtual ~IMethod();
    virtual void Signature(char* szSignature) = 0;
    virtual int Encrypt(const char* szIn, char* szOut, size_t iLength) = 0;
    virtual int Decrypt(const char* szIn, char* szOut, size_t iLength) = 0;
    virtual void ResetChain() = 0;

    void Xor(char* buff, const char* chain);
    static void BytesToWord(const unsigned char* pucBytes, unsigned int& ruiWord);
    static void WordToBytes(unsigned int uiWord, unsigned char* pucBytes);
    void SetMode(int iMode);
    void SetPadding(int iPadding);
    int GetKeyLength();
    int GetBlockSize();
    int GetMode();
    int GetPadding();
    int Pad(char* in, int iLength);

protected:
    bool m_bInit;
    int m_blockSize;
    int m_keylength;
    int m_iMode;
    int m_iPadding;

    static const char* sm_szErrorMsg1;
    static const char* sm_szErrorMsg2;
    static const char* sm_szErrorMsg3;
    static const char* sm_szErrorMsg4;
    static const char* sm_szErrorMsg5;
    static const char* sm_szErrorMsg6;
    static const char* sm_szErrorMsg7;
    static const char* sm_szErrorMsg8;
    static const char* sm_szErrorMsg9;
    static const char* sm_szErrorMsg10;
};

} // namespace nsl

#endif // NSL_METHOD_H_
