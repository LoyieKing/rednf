// ORIG 0x081c4256 T _ZN21DisPatcher_LeaveParty7processEP5CUserR8MSG_BASER9ParamBase
// param+0x4 = check_error；>0 返回 0；<0 LineFunc(0x109f,pretty,err,acc_id) 并返回其值；
// 否则 leave_user(user,2)==1 时 param=1，随后 pvp_assault 强制移动，返回 0。
// BLOCKED（缺依赖，ORIG 符号无还原实现）：
//   pvp_assault::GetInstanceAssaultMgr()->ForcedMoveAtPowerWarPlace(user);
//   - _ZN11pvp_assault11CAssaultMgr25ForcedMoveAtPowerWarPlaceEP5CUser @0x082ed672
//     （try/catch 薄包装，转调 CAssaultPlace 同名函数）
//   - _ZN11pvp_assault13CAssaultPlace25ForcedMoveAtPowerWarPlaceEP5CUser @0x082ed6a6
//     （依赖未建模类 CForcedMoveCharacter）
//   - _ZN11pvp_assault20CForcedMoveCharacter10ForcedMoveEv @0x082e567c
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 59 / ours 59 insn 全等（AE 口径，重定位归一）。
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
