int Inter_PvPChannelInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_PvPChannelInfoPktView* pPkt = reinterpret_cast<Inter_PvPChannelInfoPktView*>(data);
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;

    int* msg =&pPkt->m_0;   // ORIG 写回 msg+(i+1)*0x10+0x10，须非 const
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
        packet.put_int(*(msg + (i + 1) * 4 + 2));    // msg+(i+1)*0x10+0x08
        packet.put_int(*(msg + (i + 1) * 4 + 3));    // msg+(i+1)*0x10+0x0c
        if (msg[(i + 1) * 4 + 5] == 2)               // msg+(i+1)*0x10+0x14
            msg[(i + 1) * 4 + 4] = 4;                // msg+(i+1)*0x10+0x10 写回 4
        packet.put_int(msg[(i + 1) * 4 + 4]);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
