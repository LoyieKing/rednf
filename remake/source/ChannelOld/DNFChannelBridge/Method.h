#ifndef METHOD_H_
#define METHOD_H_

class IMethod
{
public:
    IMethod()
    {
    }
    virtual ~IMethod()
    {
    }
    virtual void Signature(char* pcSig)
    {
    }

protected:
    bool m_bInit;
    int m_blockSize;
    int m_keylength;
    int m_iMode;
    int m_iPadding;
};

class CTEA : public IMethod
{
public:
    CTEA()
    {
    }
    virtual ~CTEA()
    {
    }

    static const char* sm_chain0;
};

#endif // METHOD_H_
