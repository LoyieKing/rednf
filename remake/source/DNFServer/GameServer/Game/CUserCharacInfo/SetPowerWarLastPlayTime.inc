void CUserCharacInfo::SetPowerWarLastPlayTime(long t)
{
    if (m_selected)
    {
    CUR->m_powerWarLastPlayTime = t;
    }
}