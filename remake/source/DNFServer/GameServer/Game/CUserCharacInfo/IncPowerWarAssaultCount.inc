void CUserCharacInfo::IncPowerWarAssaultCount()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    ++m_selected->m_field1221;
    }
}