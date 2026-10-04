int Inter_SecedePower::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_SecedePowerPktView* pPkt = reinterpret_cast<Inter_SecedePowerPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    // [b28d-lane2] ORIG 084d49f6 段：比较内联 + logFormat 逐点重读 ⇒ 原码无 characNo 具名
    // 局部（P0：无槽往返），旧具名形多发 store+load 各一。
    if (user->get_charac_no(-1) != pPkt->m_characNo)         // msg+0x12
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x3620,
            "Inter_SecedePower::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharNo(%d)",
            user->get_charac_no(-1), pPkt->m_characNo);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0xb5);
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

    {   // STGuildProxy(GetGuildDBInfo(), 名随写字节+0x1b, 0)
        // ORIG 0x84d4b2f-0x84d4b41：单次调用 + `mov %eax,-0xc` 槽往返 ⇒ 具名局部（p167 E=GetGuildDBInfo×1）。
        const STGuildDBInfoOnly* guildDbInfo =
            static_cast<const STGuildDBInfoOnly*>(user->GetGuildDBInfo());
        STGuildProxy proxy(
            guildDbInfo->m_guildName,
            guildDbInfo->m_fundLevel,
            (unsigned char)0);
        // [G2A-PC 手法2/P324] ORIG 084d4b65 `call get_charac_guildkey` → 084d4b6a
        //   `mov %eax,%ebx`（跨两调用保活，无栈槽）→ 084d4b6c/084d4b72 `call GetServerGroup`
        //   → 084d4b84 `call GetServerProxy`：即先算实参、后算 receiver 链。
        //   具名 proxySrv 局部会多发 `mov %eax,-0xNN(%ebp)` + `mov -0xNN(%ebp),%eax`
        //   往返（本 diff 的 N 侧尾部即此形）⇒ 原码 receiver 为链式单表达式。
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->ModifyGuildInfoProxy((unsigned int)user->get_charac_guildkey(), proxy);
    }
    return 0;
}