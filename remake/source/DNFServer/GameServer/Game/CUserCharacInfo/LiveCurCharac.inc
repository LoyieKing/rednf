void CUserCharacInfo::LiveCurCharac()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_fieldE66 = 0;
    m_selected->m_fielde5d = 0;
    m_selected->m_fieldE61 = 0;
    if (!GlobalData::s_power_manager->IsPowerWarEventOn() ||
        getCurCharacVill() != 7)
    {
        m_selected->m_stamina = m_selected->m_pad76;
    }
    }
}