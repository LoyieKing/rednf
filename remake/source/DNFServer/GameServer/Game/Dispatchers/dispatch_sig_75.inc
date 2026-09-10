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
    // X1 按位判据（同 GuildCargoPopItem 位 B）：ORIG 该点紧随的是 `test %eax,%eax; je`
    // 直分支、无 `sete %al` 物化岛 ⇒ 此处必须写 `== 0`；我方原 `!intcallee()` 在 4.4
    // 恒产 `test;sete %al;test %al,%al` 两段岛（台账 ours_only=sete1,test1）。
    if (assaultMgr->OnDieUser(user, (int)killerIdx, msg) == 0) {
        return LineFunc(0xa1f1, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
