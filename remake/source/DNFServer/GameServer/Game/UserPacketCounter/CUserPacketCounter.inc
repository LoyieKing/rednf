CUserPacketCounter::CUserPacketCounter(char* dir, char* name)
{
    m_totalCount = 0;
    sprintf(m_logPath, "./log/%s/%s", dir, name);
