void CUserCharacInfo::setCurCharacLastPlayTick(long tick)
{
    enableSaveCharacStat();
    CUR->m_lastPlayTick = tick;
}