int Inter_MonitorDelBuddy::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG 0x084d13xx：包基址在 get_state 检查之后物化入 -0x10(%ebp)
    Inter_MonitorDelBuddyPktView* pPkt = reinterpret_cast<Inter_MonitorDelBuddyPktView*>(data);
    if (user->getCurCharacNo() !=
        pPkt->m_a)         // msg+0x0a 角色号校验
        return 0;

    PacketGuard packet;
    if (pPkt->m_30 == 0)   // msg+0x30 成功
    {
        packet.clear();
        packet.put_header(1, 0x40);
        packet.put_byte(1);
        const int nameLen =                          // msg+0x12 被删好友名
            static_cast<int>(strlen(pPkt->m_12));
        packet.put_int(nameLen);
        packet.put_str(pPkt->m_12, nameLen);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        packet.clear();
        packet.put_header(1, 0x40);
        packet.put_byte(0);
        packet.put_byte(                             // msg+0x30 char（movsbl）
            static_cast<int>(pPkt->m_30));
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}