void CUserCharacInfo::ResetPowerWarAssaultRecord()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_field1221 = 0;
    m_selected->m_field1225 = 0;
    }
}