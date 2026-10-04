int Inter_ReplyGuildCreate::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ReplyGuildCreatePktView* pPkt = reinterpret_cast<Inter_ReplyGuildCreatePktView*>(data);
    if (user->get_state() <= 2)                        // cmp $2; setle
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x3464,
            "GUILD : Inter_ReplyGuildCreate:: recv->m_uCharID(%d)",
            pPkt->m_characNo);
        return 0;
    }

    if (user->get_charac_no(-1) != pPkt->m_characNo)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x346a,
            "GUILD : Inter_ReplyGuildCreate:: : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_characNo);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x47);
    if (pPkt->m_result == 0)
    {   // 建会成功
        packet.put_byte(1);
        user->m_historyLog.GuildCreate(
            (unsigned int)user->get_charac_guildkey(),
            pPkt->m_guildName,   // ORIG 经 -0x10 槽
            pPkt->m_characNo);
    }
    else
    {   // 建会失败：退还建会费用
        packet.put_byte(0);
        packet.put_byte(pPkt->m_result);
        int createCost = G_CDataManager()->m_guildParameter.m_field0;   // +0xa50c（建会费用）
        user->getCurCharacInvenW()->gain_money(createCost,
                                               (eMoneyAddReason)0x19, true, 0);
        user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
    }
    packet.finalize(true);
    user->Send(packet);

    if (pPkt->m_result == 0)
    {   // 公会代理缓存 + 全服广播
        STGuildProxy proxy;
        proxy.m_level = 0;
        memcpy(proxy.m_guildName, pPkt->m_guildName, 0x16);  // ORIG 经 -0x10 槽
        proxy.m_powerSide = 0;
        user->SetGuildInfoProxy(proxy);

        packet.clear();                                // ORIG InterfacePacketBuf::clear
        packet.put_header(0, 2);
        packet.put_byte(0);
        packet.put_short(1);
        user->make_basic_info((char*)&packet, 0);
        packet.finalize(true);
        G_GameWorld()->send_all(packet, user);
    }
    return 0;
}