Exception::Exception(const char* szErrorMsg)
{
    iErrorCode_ = 0;
    nLine_ = 0;
    szFunctionNameA_ = NULL;
