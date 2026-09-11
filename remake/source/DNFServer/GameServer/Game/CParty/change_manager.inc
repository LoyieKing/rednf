// ============================================================================
// change_manager / IsExistInvisible / battle_data_reset / 发送族
// ============================================================================
// CParty::change_manager @ 0x859c832 —— 清空旧 manager 槽位，选举新 manager。
// 返回 1=成功 / 0=队伍已空（写日志）。
bool CParty::change_manager()
{
    int i;
    for (i = 0; i <= 3; ++i)
    {
        if (m_member[i].m_pUser == m_manager)
        {
            m_manager->SetPartyIndex(-1);
            m_manager->set_state(ch_state3);
            m_member[i].m_pUser = 0;
            m_member[i].m_field8 = 0;
            break;
        }
    }
    // ORIG 两循环各自槽位（-0x10/-0xc）：第二个循环是独立变量 j。
    int j;
    for (j = 0; j <= 3; ++j)
    {
        if (_checkValidUser(j))
        {
            m_manager = m_member[j].m_pUser;
            set_host();
            send_host_info();
            return 1;
        }
    }
    LogManager::logFormat(1, "party.cpp", __PRETTY_FUNCTION__,
                          0x689, "there is no manager(party empty)");
    return 0;
}