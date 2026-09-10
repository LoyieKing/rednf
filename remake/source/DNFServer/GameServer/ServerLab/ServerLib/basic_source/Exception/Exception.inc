Exception::Exception(int iErrorCode)
{
    iErrorCode_ = iErrorCode;
    nLine_ = 0;
    szFunctionNameA_ = NULL;
    szErrorMsg_[0] = '\0';
