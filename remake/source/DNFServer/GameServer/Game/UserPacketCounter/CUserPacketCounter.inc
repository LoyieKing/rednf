// ORIG W 0x0847e8d2：map 默认构造（编译器生成）+ total=0 +
// sprintf(m_logPath, "./log/%s/%s", dir, name)。
CUserPacketCounter::CUserPacketCounter(char* dir, char* name)
{
    m_totalCount = 0;
    sprintf(m_logPath, "./log/%s/%s", dir, name);
}