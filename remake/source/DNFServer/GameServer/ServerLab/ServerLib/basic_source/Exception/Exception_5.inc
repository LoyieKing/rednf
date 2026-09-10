Exception::Exception(const char* szFunctionNameA, int nLine, const char* szMessage, ...)
{
    nLine_ = nLine;
    szFunctionNameA_ = szFunctionNameA;

    va_list args;
    va_start(args, szMessage);
    vsprintf(szErrorMsg_, szMessage, args);
    va_end(args);
