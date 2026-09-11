int Inter_MonitorNoticeOtherChannelChat::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    char* msg = data;                                 // -0x1c

    if (reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_35 == 1)
    {
        PacketGuard packet;                            // -0x34
        packet.put_header(0, 0x41);
        packet.put_byte(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_a);
        packet.put_byte(0x15);
        const int len = (int)strlen(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_17);         // -0x14
        packet.put_int(len);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_17, len);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else if (reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_35 == 2)
    {
        PacketGuard packet;                            // -0x40
        packet.put_header(0, 0x41);
        packet.put_byte(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_a);
        packet.put_byte(0x4d);
        const int len = (int)strlen(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_17);         // -0x10
        packet.put_int(len);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_17, len);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else if (reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_35 == 3)
    {
        PacketGuard packet;                            // -0x4c
        packet.put_header(0, 0x41);
        packet.put_byte(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_a);
        packet.put_byte(0x5a);
        const int len = (int)strlen(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_17);         // -0xc
        packet.put_int(len);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_17, len);
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
            reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_13)
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x25ee,
                "Inter_MonitorNoticeBuddyChat::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
                user->get_charac_no(-1),
                reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_13);
            return 0;
        }

        PacketGuard packet;                            // -0x28
        packet.put_header(0, 0x41);
        packet.put_byte(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_a);
        packet.put_byte(0);
        const int len = (int)strlen(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_17);         // -0x18
        packet.put_int(len);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_17, len);
        packet.put_byte((int)reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_e);
        packet.put_int((int)(unsigned int)reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_36);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_37,
                       (int)(unsigned int)reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktViewI*>(msg)->m_36);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;

// [G2B-Monitor] 本件已做：+0x13 字段 → unsigned int（自带同布局视图，P248），
//   diff 的 `mov %eax,%edx` 4 行比较形随之消除。
//   残余留 N：尾段 4 条指令位置差（ORIG `mov $0x0,%ebx` 在 call 前、我方在后；
//   ndiff 里表现为 OURS 段整体后移 4）＝ return 0 常量装载与 PacketGuard 析构/Send
//   的相对次序，属 gcc 语句块尾布局，无源码旋钮可判 ⇒ 留 N，不改。
}