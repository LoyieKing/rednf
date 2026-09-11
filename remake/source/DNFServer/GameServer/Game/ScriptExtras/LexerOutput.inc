// ORIG T _Z11LexerOutputPKci 0x88bb37c：词法输出走 LogManager 日志。
void LexerOutput(const char* buf, int size)
{
    (void)size;
    LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x12f, "%s - %s",
                          __FUNCTION__, toTString(buf));
}