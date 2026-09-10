int DisPatcher_LeaveParty::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    stLeavePartyParam& out = static_cast<stLeavePartyParam&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG `mov %eax,%edx; mov -0x14(%ebp),%eax;
    // mov %edx,0x4(%eax)`（RHS-first+edx 中转）由「unsigned 返回 + 无具名 err/dst 局部」复现。
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x109f,
                        __PRETTY_FUNCTION__,
                        out.m_errNo, user->get_acc_id());
    }

    // ORIG 0x081c4256：party 具名槽 -0x10（`mov %eax,-0x10; mov -0x10,%eax` 重载传参）
    // + leave_user 结果槽 -0xc（`mov %eax,-0xc; cmpl $1,-0xc; jne` 技9 具名槽融合形）。
    // 声明序 out→party→ret（技21 先声明占远槽）。
    CParty* party = static_cast<CParty*>(user->GetParty());
    int ret = party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);  // ORIG 不判空直接调用
    if (ret == 1)
    {
        out.m_errNo = 1;
    }
    pvp_assault::GetInstanceAssaultMgr()->ForcedMoveAtPowerWarPlace(user);  // ORIG 0x082350a0
    return 0;
}
