Exception::Exception(const char* szFunctionNameA, int nLine, int iErrorCode)
{
    iErrorCode_ = iErrorCode;
    nLine_ = nLine;
    szFunctionNameA_ = szFunctionNameA;
