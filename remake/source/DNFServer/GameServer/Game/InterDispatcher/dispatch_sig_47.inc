int Inter_MonitorNoticeOtherChannelChat::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    char* msg = data;                                 // -0x1c

    if (reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_35 == 1)
    {
        PacketGuard packet;                            // -0x34
        packet.put_header(0, 0x41);
        packet.put_byte(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_a);
        packet.put_byte(0x15);
        const int len = (int)strlen(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17);         // -0x14
        packet.put_int(len);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17, len);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else if (reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_35 == 2)
    {
        PacketGuard packet;                            // -0x40
        packet.put_header(0, 0x41);
        packet.put_byte(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_a);
        packet.put_byte(0x4d);
        const int len = (int)strlen(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17);         // -0x10
        packet.put_int(len);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17, len);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else if (reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_35 == 3)
    {
        PacketGuard packet;                            // -0x4c
        packet.put_header(0, 0x41);
        packet.put_byte(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_a);
        packet.put_byte(0x5a);
        const int len = (int)strlen(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17);         // -0xc
        packet.put_int(len);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17, len);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else
    {
        if (user->get_charac_no(-1) !=
            reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_13)
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x25ee,
                "Inter_MonitorNoticeBuddyChat::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
                user->get_charac_no(-1),
                reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_13);
            return 0;
        }

        PacketGuard packet;                            // -0x28
        packet.put_header(0, 0x41);
        packet.put_byte(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_a);
        packet.put_byte(0);
        const int len = (int)strlen(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17);         // -0x18
        packet.put_int(len);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17, len);
        packet.put_byte((int)reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_e);
        packet.put_int((int)(unsigned int)reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_36);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_37,
                       (int)(unsigned int)reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_36);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
