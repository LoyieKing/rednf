// ORIG T _Z10LexerErrorPKc 0x88bb3c7：词法错误走 LogManager 日志。
void LexerError(const char* msg)
{
    LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x134, "ERROR! %s",
                          toTString(msg));
}