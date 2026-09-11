// ---- CParty::walkout_member（ORIG T 0x859ca4a）----
int CParty::walkout_member(CUser* user, int slot)
{
    // b28d-49 @0x859ca4a：ORIG 先把 m_member[slot].m_pUser 读入 -0x10（target 局部），
    // 随后**重算地址二次读**该成员作为 leave_user 实参（P157/P0 逐点重读，不缓存传参）；
    // ForcedMoveAtPowerWarPlace 用 target；GetInstanceAssaultMgr() 结果直接 %eax 压 this
    //（不落 mgr 局部，ORIG 0859cae7 无槽中转）。
    if (m_manager != user)
        return 8;
    if (m_member[slot].m_pUser != 0)
    {
        CUser* target = m_member[slot].m_pUser;
        int r = leave_user(m_member[slot].m_pUser, (ENUM_PARTY_INFO_TYPE)2);
        if (r == 0)
        {
            pvp_assault::GetInstanceAssaultMgr()->ForcedMoveAtPowerWarPlace(target);
            return 0;
        }
        if (r == 1)
            return 1;
    }
    return 4;
}