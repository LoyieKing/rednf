int Inter_MonitorNoticeOtherChannelChatHyperLink::dispatch_sig(
    CUser* user, char* data, int /*len*/)
{
    Inter_MonitorNoticeOtherChannelChatHyperLinkPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeOtherChannelChatHyperLinkPktView*>(data);
    // ORIG 0x084e5528 报告 Inter_MonitorNoticeOtherChannelChatHyperLink/dispatch_sig.md
    // 两段全读。四分支按 msg[0x35]（1/2/3/其余）各组成 0x173 通知包。
    if (user->get_state() == 0)
        return 0;

    // §P0：ORIG 对 m_35/m_36 全程重读（无具名局部槽往返）⇒ 不缓存 linkType/itemCount/count。
    if (pPkt->m_35 == 1)
    {
        PacketGuard packet;
        packet.put_header(0, 0x173);
        packet.put_byte(pPkt->m_a);
        packet.put_byte(0x15);
        // ORIG 每分支单次 strlen（p167 EXTRA strlen×4 实证）⇒ 具名缓存。
        const int strLen = static_cast<int>(strlen(pPkt->m_17));
        packet.put_int(strLen);
        packet.put_str(pPkt->m_17, strLen);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.put_byte(pPkt->m_36);
        for (int i = 0; i < (int)pPkt->m_36; ++i)
            packet.put_binary(data + i * 0x68 + 0x37, 0x68);
        packet.finalize(true);
        user->Send(packet);
    }
    else if (pPkt->m_35 == 2)
    {
        PacketGuard packet;
        packet.put_header(0, 0x173);
        packet.put_byte(pPkt->m_a);
        packet.put_byte(0x4d);
        const int strLen = static_cast<int>(strlen(pPkt->m_17));
        packet.put_int(strLen);
        packet.put_str(pPkt->m_17, strLen);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.put_byte(pPkt->m_36);
        for (int i = 0; i < (int)pPkt->m_36; ++i)
            packet.put_binary(data + i * 0x68 + 0x37, 0x68);
        packet.finalize(true);
        user->Send(packet);
    }
    else if (pPkt->m_35 == 3)
    {
        PacketGuard packet;
        packet.put_header(0, 0x173);
        packet.put_byte(pPkt->m_a);
        packet.put_byte(0x5a);
        const int strLen = static_cast<int>(strlen(pPkt->m_17));
        packet.put_int(strLen);
        packet.put_str(pPkt->m_17, strLen);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.put_byte(pPkt->m_36);
        for (int i = 0; i < (int)pPkt->m_36; ++i)
            packet.put_binary(data + i * 0x68 + 0x37, 0x68);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        // ORIG 0x84e5a18/0x84e5a42：两处独立 get_charac_no 调用（P48，无具名缓存）。
        if (user->get_charac_no(-1) != pPkt->m_13)
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                "virtual int Inter_MonitorNoticeOtherChannelChatHyperLink::"
                "dispatch_sig(CUser*, char*, int)", 0x6497,
                "Inter_MonitorNoticeOtherChannelChatHyperLink::dispatch_sig() : "
                "pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
                user->get_charac_no(-1), pPkt->m_13);
            return 0;
        }

        PacketGuard packet;
        packet.put_header(0, 0x173);
        packet.put_byte(pPkt->m_a);
        packet.put_byte(0);
        const int strLen = static_cast<int>(strlen(pPkt->m_17));
        packet.put_int(strLen);
        packet.put_str(pPkt->m_17, strLen);
        packet.put_byte(0);
        // ORIG 084e5b28/084e5b44：0x16f 按 1 字节读（movzbl），零扩展传值。
        packet.put_int(pPkt->m_16f);
        packet.put_str(pPkt->m_170,
                       pPkt->m_16f);
        packet.put_byte(pPkt->m_36);
        for (int i = 0; i < (int)pPkt->m_36; ++i)
            packet.put_binary(data + i * 0x68 + 0x37, 0x68);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
