void CUserCharacInfo::LiveCurCharac()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_ghost = 0;
    m_selected->m_chaosDieTime = 0;
    m_selected->m_chaosStateTime = 0;
    if (!GlobalData::s_power_manager->IsPowerWarEventOn() ||
        getCurCharacVill() != 7)
    {
        m_selected->m_stamina = m_selected->m_pad76;
    }
    }
}