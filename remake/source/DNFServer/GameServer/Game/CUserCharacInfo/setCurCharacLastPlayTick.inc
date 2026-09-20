void CUserCharacInfo::setCurCharacLastPlayTick(long tick)
{
    enableSaveCharacStat();
    m_selected->m_field7b = tick;
}