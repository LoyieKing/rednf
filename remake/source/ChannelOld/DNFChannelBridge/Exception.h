#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include "stdafx.h"

class Exception
{
public:
    Exception(int iErrorCode);
    Exception(const TCHAR* szErrorMsg);
    Exception(const char* szFunctionNameA, int nLine);
    Exception(const char* szFunctionNameA, int nLine, int iErrorCode);
    Exception(int iErrorCode, const TCHAR* szMessage, ...);
    Exception(const char* szFunctionNameA, int nLine, const TCHAR* szMessage, ...);

    int getErrorCode() const;
    TCHAR* getErrorMsg() const;
    const char* getFunctionName() const;
    int getLine() const;

private:
    int nLine_;
    int iErrorCode_;
    TCHAR szErrorMsg_[8192];
    const char* szFunctionNameA_;
};

#endif // EXCEPTION_H_
