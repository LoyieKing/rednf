int Inter_PvPChannelInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_PvPChannelInfoPktView* pPkt = reinterpret_cast<Inter_PvPChannelInfoPktView*>(data);
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;

    // P0/P183：ORIG 无 msg 具名槽（各用点重新 `mov 0x10(%ebp),%eax`），且地址按
    //   char* 基址 + (i+1)*0x10 + off 计算（形如 `mov 0x8(%edx,%eax,1)`），不是 int* 指针算术。
    if (user->getCurCharacR() == 0 ||                // asm 双失败归并 true
        user->getCurCharacNo() !=
            pPkt->m_a)     // msg+0x0a 角色号
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0xc6);
    packet.put_byte(1);
    packet.put_int(pPkt->m_12);  // msg+0x12
    const unsigned int channelCnt =                  // msg+0x17 频道数
        static_cast<unsigned int>(pPkt->m_17);
    packet.put_byte(static_cast<int>(channelCnt));
    for (int i = 0; i < static_cast<int>(channelCnt); ++i)
    {
        packet.put_int(*reinterpret_cast<int*>(data + (i + 1) * 0x10 + 0x8));
        packet.put_int(*reinterpret_cast<int*>(data + (i + 1) * 0x10 + 0xc));
        if (*reinterpret_cast<int*>(data + (i + 1) * 0x10 + 0x14) == 2)
            *reinterpret_cast<int*>(data + (i + 1) * 0x10 + 0x10) = 4;  // 写回 4
        packet.put_int(*reinterpret_cast<int*>(data + (i + 1) * 0x10 + 0x10));
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
