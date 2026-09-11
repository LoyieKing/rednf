// [b28c-L19 属主修复] 参数名曾被注释但函数体用 &msg ⇒ TU 编译失败，恢复具名。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 71 / ours 71 insn 全等（AE 口径，重定位归一）。
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
