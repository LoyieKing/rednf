void CUserCharacInfo::LiveCurCharac()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_ghost = 0;
    CUR->m_chaosDieTime = 0;
    CUR->m_chaosStateTime = 0;
    if (!GlobalData::s_power_manager->IsPowerWarEventOn() ||
        getCurCharacVill() != 7)
    {
        CUR->m_stamina = CUR->m_backupStamina;
    }
    }
