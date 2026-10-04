int Inter_ReplyJoinPower::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;
    // [P28/ORIG 084d47ef] ORIG 把 data 形参缓存（`mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`）
    //   放在 get_state 检查**之后** ⇒ 声明与赋值同置于检查后（槽序照 ORIG）。
    Inter_ReplyJoinPowerPktView* pPkt = reinterpret_cast<Inter_ReplyJoinPowerPktView*>(data);

    // [b28d-lane2] ORIG 084d47f4 段：比较为内联形（`call get_charac_no; mov -0x10(%ebp),%eax;
    // mov 0x12(%eax),%eax; cmp %eax,%edx`），logFormat 两处亦逐点重读 ⇒ 原码无 characNo
    // 具名局部（P0：无槽往返），旧具名形多发 store+load 各一。
    if (user->get_charac_no(-1) != pPkt->m_characNo)         // msg+0x12
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x35fc,
            "Inter_ReplyJoinPower::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharNo(%d)",
            user->get_charac_no(-1), pPkt->m_characNo);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0xb4);
    if (pPkt->m_result == 0)
    {
        packet.put_byte(1);
        packet.put_byte(
            (int)pPkt->m_16);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(pPkt->m_result);
    }
    packet.finalize(true);
    user->Send(packet);

    {   // STGuildProxy(GetGuildDBInfo(), 名随写字节+0x1b, msg+0x16)
        const STGuildDBInfoOnly* guildDbInfo =
            static_cast<const STGuildDBInfoOnly*>(user->GetGuildDBInfo());
        STGuildProxy proxy(
            guildDbInfo->m_guildName,
            guildDbInfo->m_fundLevel,
            pPkt->m_16);
        // [G2A-PC 手法2/P324] ORIG 084d4969 `call get_charac_guildkey` → 084d496e
        //   `mov %eax,%ebx`（跨两调用保活，无栈槽）→ 084d4976/084d4988 receiver 链
        //   （GetServerGroup → GetServerProxy）→ 084d499b call ModifyGuildInfoProxy：
        //   先算实参、后算 receiver ⇒ 原码无具名 proxySrv 局部（具名形多 store+load 往返）。
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->ModifyGuildInfoProxy((unsigned int)user->get_charac_guildkey(), proxy);
    }
    return 0;
}