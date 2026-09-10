bool CUser::Send(PacketGuard& packet)   // b28c-L5 SIGFIX：ORIG 串 bool；
                                        // 调用点 0x082e6c2b `xor$1;test%al` bool callee 直判
{
    // 2026-09-10 gA（A 组）P0/P87/P183 三轴：
    //  · ORIG 0x086485da `test %al,%al; je 086486ab`（**无 xor$1**，成功块 fall-through、
    //    失败块在 0x086486ab）⇒ 源码是正向 if/else，非 `if (!f())`（P168a③）。
    //  · 失败块 ORIG 0x086486be `movzwl (%eax),%eax; movzwl %ax,%ebx`（packetId 无槽、
    //    跨 getCurCharacName 保活 %ebx）、name 直留 %eax ⇒ 删具名 packetId/name。
    //  · 成功块 ORIG 0x0864861e packetId 保活 %ebx、0x08648634 `movzbl %al,%eax`（area
    //    直传）、0x08648658 `cmp $0x6,%al; setne %al; test %al,%al; je`（b 直判）
    //    ⇒ 删具名 area/b，全部实参内联。
    PacketBuf* pBuf = ((InterfacePacketBuf*)&packet)->get();
    if (pBuf->is_finallized())
    {
        // ORIG 0x086485e2/0x086485f5：两个死 PacketBuf::at() 直调（结果丢弃，P184 保留）
        pBuf->at(0);
        pBuf->at(1);
        GetPacketName((ENUM_PACKETCLASS)pBuf->at(0),
                      *((unsigned short*)pBuf->get_packet(1)));
        if (pBuf->at(1) != 6)
        {
            // ORIG 块内 at(1)/at(0) 各重调一次，非复用上面的值
            m_pad8cf0c[0] = pBuf->at(1);
            m_pad8cf0c[1] = pBuf->at(0);
        }
        return AntibotSend(*pBuf);
    }
    LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__, 0x8a0,
                          "User %s - Packet.is_finallized packet_id(%d) return false",
                          getCurCharacName(),
                          (int)*((unsigned short*)pBuf->get_packet(1)));
    return 0;
