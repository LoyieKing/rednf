int Inter_SecedePower::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_SecedePowerPktView* pPkt = reinterpret_cast<Inter_SecedePowerPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    // [b28d-lane2] ORIG 084d49f6 段：比较内联 + logFormat 逐点重读 ⇒ 原码无 characNo 具名
    // 局部（P0：无槽往返），旧具名形多发 store+load 各一。
    if (user->get_charac_no(-1) != pPkt->m_12)         // msg+0x12
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x3620,
            "Inter_SecedePower::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharNo(%d)",
            user->get_charac_no(-1), pPkt->m_12);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0xb5);
    if (pPkt->m_a == 0)
    {
        packet.put_byte(1);
        packet.put_byte(
            (int)pPkt->m_16);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(pPkt->m_a);
    }
    packet.finalize(true);
    user->Send(packet);

    {   // STGuildProxy(GetGuildDBInfo(), 名随写字节+0x1b, 0)
        // ORIG 0x84d4b2f-0x84d4b41：单次调用 + `mov %eax,-0xc` 槽往返 ⇒ 具名局部（p167 E=GetGuildDBInfo×1）。
        const char* guildDbInfo = static_cast<const char*>(user->GetGuildDBInfo());
        STGuildProxy proxy(
            guildDbInfo,
            *(reinterpret_cast<const unsigned char*>(guildDbInfo) + 0x1b),
            (unsigned char)0);
        CGuildServerProxy* proxySrv =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxySrv->ModifyGuildInfoProxy(
            (unsigned int)user->get_charac_guildkey(), proxy);
    }
    return 0;
