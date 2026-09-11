// ORIG 0x0826503c T _ZN21DisPatcher_FinishLoad7processEP5CUserR8MSG_BASER9ParamBase
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 113 / ours 113 insn 全等（AE 口径，重定位归一）。
int DisPatcher_FinishLoad::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    stFinishLoadParam& out = static_cast<stFinishLoadParam&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG `mov %eax,%edx; mov <out>,%eax; mov %edx,0x4(%eax)`。
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo >= 1)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {   // LineFunc(0x6ca, pretty, m_errNo, 0)——第四参恒 0（ORIG 无 get_acc_id 调用）
        return LineFunc(0x6ca,
                        __PRETTY_FUNCTION__,
                        out.m_errNo, 0);
    }
    if (Global::CBossTowerFunc::FinishLoad(user))   // 死亡塔(0xa)/Boss 塔(0xc) 自行收尾
    {
        return 0;
    }
    if (advancealtar::Manager::finishLoad(user))    // 祭坛进行中则接管
    {
        return 0;
    }
    Party_hack_check(user);
    MSG_FINISH_LOADING& req = *reinterpret_cast<MSG_FINISH_LOADING*>(&msg);
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        party->finish_loading(user);
        if (party->checkStartRoom())
        {   // 开房校验后核对客户端 checksum，不符记 0x25b/0x25c
            if (user->getCurCharacInvenCheckSum() != req.m_0d)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x25b, 1, 0, 0);
            }
            if (user->getCurCharacSkillCheckSum() != req.m_11)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x25c, 1, 0, 0);
            }
        }
    }
    return 0;
}
