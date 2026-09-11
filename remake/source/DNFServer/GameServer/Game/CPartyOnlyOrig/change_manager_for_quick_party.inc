// ORIG T 0x0859b404（change_manager_for_quick_party.md）：manager 本人以
// PARTY_INFO_TYPE 无关路径摘出（SetPartyIndex(-1) 并清槽），再选首个有效
// 成员为新 host/manager；找不到则记录 no manager 日志返回 0。
bool CParty::change_manager_for_quick_party()
{
    for (int i = 0; i <= 3; i++)
    {
        if (m_member[i].m_pUser == m_manager)
        {
            m_manager->SetPartyIndex(-1);
            m_member[i].m_pUser = 0;
            m_member[i].m_field8 = 0;
            break;
        }
    }
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i))
        {
            m_manager = m_member[i].m_pUser;
            set_host();
            send_host_info();
            return 1;
        }
    }
    LogManager::logFormat(
        1, "party.cpp", __PRETTY_FUNCTION__, 0x45d,
        "there is no manager(party empty)");
    return 0;
}