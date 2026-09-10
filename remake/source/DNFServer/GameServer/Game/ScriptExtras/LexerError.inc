void LexerError(const char* msg)
{
    LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x134, "%s",
                          toTString(msg));
