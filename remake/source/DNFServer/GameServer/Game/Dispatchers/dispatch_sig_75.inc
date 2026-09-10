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
    pvp_assault::CAssaultMgr* assaultMgr = pvp_assault::GetInstanceAssaultMgr();
    // ORIG 0x0820..（[131] 处直证）：`xor $0x1,%eax` + 紧随 `test %al,%al` ⇒ 原码是
    //   `!f()`；旧形 `f() == 0` 令 4.4 产 `test %eax,%eax; sete %al` 两段（+1 条、ndiff）。
    if (!assaultMgr->OnDieUser(user, (int)killerIdx, msg)) {
        return LineFunc(0xa1f1, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
