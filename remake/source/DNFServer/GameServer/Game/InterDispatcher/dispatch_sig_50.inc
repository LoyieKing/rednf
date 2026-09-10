int Inter_MonitorSetGuildKey::dispatch_sig(CUser* user, char* data, int len)
{
    // [G2A-PC 本轮残留 N（已删 unused local_10）] 残留为 setg/setle 反转结构：
    //   ORIG [28:29] `setg %al` 且 `mov $0x26d4,%eax` 落在函数尾 [182:185]；
    //   本方为 `setle %al` 且 0x26d4 提前在 [34:36] ⇒ 原码是「主体在 `if (state > 2)` 内、
    //   0x26d4 作尾部 fall-through 返回」。改写需整体重排函数体（约 60 行），
    //   风险高于本件 ndiff=10 的收益，留 N。
    Inter_MonitorSetGuildKeyPktView* pPkt = reinterpret_cast<Inter_MonitorSetGuildKeyPktView*>(data);
    // [G2A-PC P0/P183 去具名局部] ORIG 无该槽往返：`const int local_10 = len;`（未使用）
    //   会多发 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` 两条（即本 diff N 侧
    //   `insert ORIG[6] OURS[6:8]`）⇒ 原码无此局部，且形参 len 自始未被引用。
    (void)len;
    if (user->get_state() == 2)                        // test eax,eax; sete
    {
        user->set_charac_guildkey_in_login(            // msg+0x12, msg+0xe
            pPkt->m_12,
            pPkt->m_e);
        return 0;
    }

    if (user->get_state() <= 2)                        // cmp $2; setg → !(state>2)
        return 0x26d4;

    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x26a7,
            "Inter_MonitorSetGuildKey::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (pPkt->m_12 == 0)
    {
        user->resetGuildDBInfo();
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0x97);
        packet.finalize(true);
        user->Send(packet);
        user->send_skill_info();

        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(1);
        packet.put_short(1);
        user->make_basic_info(reinterpret_cast<char*>(&packet), '\x01');
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        user->set_charac_guildkey(                     // CUserCharacInfo::set_charac_guildkey
            pPkt->m_12);
    }

    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(2);
        user->make_basic_info(reinterpret_cast<char*>(&packet), '\x02');
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
