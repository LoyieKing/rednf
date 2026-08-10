#ifndef EXCEPTION_H_
#define EXCEPTION_H_

// Exception：nLine_@0 / iErrorCode_@4 / szErrorMsg_[0x1000]@8 / szFunctionNameA_@0x1008
// 大小 0x100c（relay 为 4KB 缓冲版本，auction/channel 为 8KB）
class Exception
{
public:
    Exception(int iErrorCode);
    Exception(const char* szFunctionNameA, int nLine);
    Exception(const char* szFunctionNameA, int nLine, int iErrorCode);
    Exception(const char* szErrorMsg);
    Exception(int iErrorCode, const char* szMessage, ...);
    Exception(const char* szFunctionNameA, int nLine, const char* szMessage, ...);
    int getErrorCode() const
    {
        return iErrorCode_;
    }
    int getLine() const
    {
        return nLine_;
    }
    const char* getErrorMsg() const
    {
        return szErrorMsg_;
    }
    const char* getFunctionName() const
    {
        return szFunctionNameA_;
    }

private:
    int nLine_;
    int iErrorCode_;
    char szErrorMsg_[0x1000];
    const char* szFunctionNameA_;
};

#endif // EXCEPTION_H_
