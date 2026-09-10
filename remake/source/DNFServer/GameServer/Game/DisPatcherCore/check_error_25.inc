unsigned int DisPatcher_MoveMap::check_error(CUser* user, MSG_BASE& msg)

{
    if (user == 0)
    {
        return 0xfffffffd;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return 0xfffffffc;
    }
    MSG_MOVE_MAP& req = *reinterpret_cast<MSG_MOVE_MAP*>(&msg);   // ORIG -0xc(%ebp)
    // ORIG +0x45/+0x63：每次 call 现场 `mov -0x10(%ebp),%eax; add $0xb24,%eax`，
    // BF 地址不入槽（技36 删中转）；ORIG +0x77/+0x7e 双 int 常量岛 + `test %al,%al`
    // = \|\| 短路物化绑 %al（技22/40），无 invalid 槽。
    // [b28d-solo-DisPatcherCore] P0：ORIG 无 invalid 槽（尾 `test %al,%al` 直判）
    // ⇒ 原码是内联 || 表达式；具名 bool 局部使 4.4 多占 4B 槽并整体平移（-0x14/-0x10
    // vs ORIG -0x10/-0xc）。
    if (((int)req.m_y >= party->m_battleField.getMapHeight()) ||
        ((int)req.m_x >= party->m_battleField.getMapWidth()))
    {
        return 0xfffffffb;
    }
    user->lock();
    if (user != 0)
    {
        static_cast<std::set<std::pair<unsigned char, unsigned short> >*>(
            user->GetCSHashSet())
            ->clear();
    }
    user->unlock();
    if (!Global::CGlobalFunc::Get_State_Check(
            static_cast<ch_state>(user->get_state())))
    {
        return 0xffffffff;
    }
    return 0;
}
