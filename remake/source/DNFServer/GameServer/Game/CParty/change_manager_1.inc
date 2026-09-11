// ==== CParty::change_manager @ 0x0859c978（两参版，change_manager() 无参版同族报告）====
// ORIG _ZN6CParty14change_managerEP5CUseri T：仅现任 manager 可换任；
// slot 越界/空槽→21、非本人→8、目标已是 manager→18；成功换 m_manager 并广播。
int CParty::change_manager(CUser* user, int slot)
{
    if (slot < 0 || 3 < slot)
        return 0x15;
    if (m_member[slot].m_pUser == 0)
        return 0x15;
    if (user != m_manager)
        return 8;
    if (m_member[slot].m_pUser == m_manager)
        return 0x12;
    m_manager = m_member[slot].m_pUser;
    set_host();
    send_host_info();
    G_GameWorld()->send_party_info_to_all(this, 2);
    return 0;
}