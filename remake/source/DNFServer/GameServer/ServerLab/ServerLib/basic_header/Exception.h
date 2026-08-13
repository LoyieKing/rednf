#ifndef NSL_EXCEPTION_H_
#define NSL_EXCEPTION_H_

namespace nsl {

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
    int iErrorCode_;
    int nLine_;
    char szErrorMsg_[8192];
    const char* szFunctionNameA_;
};

} // namespace nsl

#endif // NSL_EXCEPTION_H_
