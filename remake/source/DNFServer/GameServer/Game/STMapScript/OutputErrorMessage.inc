// ORIG 0x088c03d6：RDARScript 错误日志输出。
void OutputErrorMessage(const char* msg)
{
    LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0xa26, msg);
}