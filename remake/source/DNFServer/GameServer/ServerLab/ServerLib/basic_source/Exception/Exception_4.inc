Exception::Exception(int iErrorCode, const char* szMessage, ...)
{
    iErrorCode_ = iErrorCode;
    nLine_ = 0;
    szFunctionNameA_ = NULL;

    va_list args;
    va_start(args, szMessage);
    vsprintf(szErrorMsg_, szMessage, args);
    va_end(args);
