int Inter_NpcLimitBuyItemInfoRenew::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NpcLimitBuyItemInfoRenewPktView* pPkt = reinterpret_cast<Inter_NpcLimitBuyItemInfoRenewPktView*>(data);
    const int itemCount = pPkt->m_a;   // msg+0x0a 条目数

    PacketGuard packet;
    packet.put_header(0, 0x193);
    packet.put_int(itemCount);
    for (int i = 0; i < itemCount && i < 0x1e; ++i)   // asm 双条件（jle/jg 归并）
    {
        packet.put_int(*reinterpret_cast<int*>(data + i * 0xc + 0xe));
        if (*reinterpret_cast<int*>(data + i * 0xc + 0x12) == 0)
        {
            packet.put_int(0);
        }
        else if (*reinterpret_cast<unsigned int*>(data + i * 0xc + 0x16) <
                 *reinterpret_cast<unsigned int*>(data + i * 0xc + 0x12))   // jb 无符号比较
        {
            packet.put_int(
                *reinterpret_cast<int*>(data + i * 0xc + 0x12) -
                *reinterpret_cast<int*>(data + i * 0xc + 0x16));
        }
        else
        {
            packet.put_int(0);
        }
    }
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
