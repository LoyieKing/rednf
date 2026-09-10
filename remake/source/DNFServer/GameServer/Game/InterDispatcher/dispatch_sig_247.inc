int Inter_ReqQueryCharacInfoMailbox::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    char* recv = data;                                 // -0xc(%ebp) 缓存消息指针

    if (reinterpret_cast<Inter_ReqQueryCharacInfoMailboxPktView*>(recv)->m_20 != 0)
    {   // 结果码非零 → 错误包
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x145,
            (unsigned char)reinterpret_cast<Inter_ReqQueryCharacInfoMailboxPktView*>(recv)->m_20);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x145);
    packet.put_byte(1);
    packet.put_int(strlen(recv));
    packet.put_str(recv, strlen(recv));                // ORIG 两次独立 strlen（P48）
    packet.put_short((int)reinterpret_cast<Inter_ReqQueryCharacInfoMailboxPktView*>(recv)->m_2a);  // movzwl+cwtl
    packet.put_byte((int)reinterpret_cast<Inter_ReqQueryCharacInfoMailboxPktView*>(recv)->m_28);
    packet.put_byte((int)reinterpret_cast<Inter_ReqQueryCharacInfoMailboxPktView*>(recv)->m_2c);
    packet.put_byte((int)reinterpret_cast<Inter_ReqQueryCharacInfoMailboxPktView*>(recv)->m_2d);
    packet.finalize(true);
    user->Send(packet);
    return 0;
