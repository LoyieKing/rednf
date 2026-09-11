// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08207208 orig/ours=146/145 ndiff-blocks=23
//   帧差 sub $34 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x34,%esp | movl   $&_ZZN27Dispatcher_DieAssaultPlayer12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
int Dispatcher_DieAssaultPlayer::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 2) {
        return LineFunc(0xa1d2, __PRETTY_FUNCTION__, 0, 0);
    }
    char killerIdx = (char)0xff;
    if (!packet.get_byte(killerIdx)) {
        return LineFunc(0xa1d6, __PRETTY_FUNCTION__, 0, 0);
    }
    MSG_DIE_ASSAULT_PLAYER msg;
    if (!packet.get_int((unsigned int&)msg.m_damage)) {
        return LineFunc(0xa1dc, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(msg.m_attackCount)) {
        return LineFunc(0xa1df, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(msg.m_field08)) {
        return LineFunc(0xa1e2, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(msg.m_field0a)) {
        return LineFunc(0xa1e5, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(msg.m_field0c)) {
        return LineFunc(0xa1e8, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(msg.m_field0e)) {
        return LineFunc(0xa1eb, __PRETTY_FUNCTION__, 0, 0);
    }
    // [G2-75 2026-09-11 负结论] 试 P52 链式 receiver（删 assaultMgr 具名槽）⇒ 实测
    //   145/145 → **145/147**（劣化 2）⇒ 回滚保具名形。ORIG 0x08207432 receiver 直用，
    //   但 4.4 对链式形多发 callee-saved 段，单 TU 不可复现。
    pvp_assault::CAssaultMgr* assaultMgr = pvp_assault::GetInstanceAssaultMgr();
    // ORIG 0x0820..（[131] 处直证）：`xor $0x1,%eax` + 紧随 `test %al,%al` ⇒ 原码是
    //   `!f()`；旧形 `f() == 0` 令 4.4 产 `test %eax,%eax; sete %al` 两段（+1 条、ndiff）。
    if (!assaultMgr->OnDieUser(user, (int)killerIdx, msg)) {
        return LineFunc(0xa1f1, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}