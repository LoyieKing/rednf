// ORIG 0x081c951c T _ZN18DisPatcher_SetArea7processEP5CUserR8MSG_BASER9ParamBase
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 94 / ours 94 insn 全等（AE 口径，重定位归一）。
int DisPatcher_SetArea::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    // ORIG 081c9525/+0x0f：入口先存 msg→-0x20（req）、后存 param→-0x1c（out）；
    // 4.4 后声明占近槽（技21），故 req 先声明。
    MSG_SET_USER_AREA& req = *reinterpret_cast<MSG_SET_USER_AREA*>(&msg);
    stSetAreaParam& out = static_cast<stSetAreaParam&>(param);
    // ORIG +0x35：`mov %eax,%edx`（value 先落 edx）→ `mov -0x1c,%eax`（地址后载）→
    // `mov %edx,0x4(%eax)`（register asm 双钉，同 Dispatchers 16628 形）。
    // [b28d-solo-DisPatcherCore] P211：ORIG RHS-first+edx 中转（unsigned 返回 + 无具名局部）。
    out.m_errNo = check_error(user, msg, param);
    if (out.m_errNo >= 1)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {   // LineFunc(0x1c6d, pretty, m_errNo, acc_id)；ORIG 直接返回其值
        return LineFunc(0x1c6d,
                        __PRETTY_FUNCTION__,
                        out.m_errNo, user->get_acc_id());
    }
    G_GameWorld()->move_area(user,
                             (int)req.m_0d,           // vill（sbyte）
                             (int)req.m_0e,           // area（sbyte）
                             (int)req.m_0f,           // x（ushort）
                             (int)req.m_11,           // y（ushort）
                             (int)req.m_13,           // z（sbyte）
                             true,                    // flag 恒 1
                             0,                       // param 恒 0
                             (int)req.m_14,           // prevVill（ushort）
                             (int)req.m_16);          // prevArea（ushort）
    return 0;
}
