void LexerOutput(const char* buf, int size)
{
    (void)size;
    LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x12f, "%s - %s",
                          __FUNCTION__, toTString(buf));
