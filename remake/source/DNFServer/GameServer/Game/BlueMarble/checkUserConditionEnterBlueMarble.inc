// [b28d-solo-BlueMarble AE + 真语义 bug] moveSpace 守卫原写反（==1 入主流程，
// ORIG 是 ==1 返回 0x17）；改 `(getMoveSpace()-1)==0` early-return 同时复现
// ORIG `sete; test; je <主流程>` 落点，175=175 ndiff 0。ORIG 0x080d8d02
int BlueMarble::checkUserConditionEnterBlueMarble(CUser* user)
{
    if (user == 0)
    {
        return 0x17;
    }
    int count = 0;
    for (int i = 0; i < m_pScript->m_info.m_playerMax; ++i)
    {
        if (m_users[i].getUser() == user)
        {
            ++count;
        }
    }
    if (count > 0)
    {
        leaveUser(user);
        return 0x12;
    }
    // ORIG +0x95 `cmp $3; setne %al; test; je <体>` / +0xcd `cmp $1; sete %al;
    // test; je <体>`：guard 物化形（P194 反形），真→fall 0x17 岛，假→je 入体。
    // b28c-1：P0/N96 清绑（P168a② 具名 bool）。
    if (user->get_state() != 3)
    {
        return 0x17;
    }
    if (user->checkInBlueMarble())
    {
        return 0x17;
    }
    // ORIG +0xcd `cmp $1,%eax; sete %al; test; je <主流程体@0x80d8de3>`：
    // 物化 (moveSpace==1)，**al==0（moveSpace!=1）跳主流程**，fall 返回 0x17。
    // [b28d-solo-BlueMarble 真语义 bug] 旧 goto 形条件写反（==1 入主流程）；
    // 4.4 对 `!(x==1)` 折成 setne，唯 `(x-1)==0` 形复现 sete+je 落点
    // （探针 /tmp/p_bm3.cpp c20/c22）。early-return 使 return 块 fall-through。
    if ((user->getMoveSpace() - 1) == 0)
    {
        return 0x17;
    }
    // P48 去缓存：ORIG +0x0e7/+0x0ff 对上下界各独立重调 get_charac_level
    //（两次调用，不缓存进 level 局部）；第二条件 call 在前（P134）。
    if (user->get_charac_level() < m_pScript->m_info.m_enterLevel ||
        user->get_charac_level() > m_pScript->m_info.m_enterMaxLevel)
    {
        return 0x17;
    }
    if ((int)user->getBlueMarbleEnterCount() >=
        m_pScript->m_info.m_enterCount)
    {
        return 0x17;
    }
    if (user->getCurCharacVill() == 7)
    {
        return 0x17;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 0x17;
    }
    if (user->IsBusyDisjointStore())
    {
        return 0x17;
    }
    if (user->IsCurCharacGhost())
    {
        return 0x17;
    }
    if (user->CheckInTrade())
    {
        return 0x17;
    }
    if (user->isCompetitionMercenary())
    {
        return 0x17;
    }
    if (user->CheckInParty())
    {
        CParty* party = (CParty*)user->GetParty();
        if (party != 0)
        {
            party->send_invite_bluemarble(user->get_unique_id() & 0xffff);
            G_CGameManager()->CheckOutParty(user, false);
        }
    }
    return 0;
}