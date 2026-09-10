Exception::Exception(const char* szFunctionNameA, int nLine)
{
    iErrorCode_ = 0;
    nLine_ = nLine;
    szFunctionNameA_ = szFunctionNameA;
    szErrorMsg_[0] = '\0';
