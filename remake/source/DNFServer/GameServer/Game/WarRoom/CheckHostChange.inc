void WarRoom::CheckHostChange()
{
    // b28d-solo-WarRoom：ORIG 0x86c0964 控制流实证 —— m_userCount<=1 时
    // `jle 0x86c0a8d` 直落重置块（m_hostVotes[i]=-1、m_userCount=0）；仅
    // m_userCount>1 且 bestSlot==-1 时经 ebx=0 跳过重置（0x86c0a5c mov $1,%ebx /
    // 0x86c0a1e mov $0,%ebx + 0x86c0a89 test %ebx,%ebx; je end）⇒ 原码是块内
    // `if (bestSlot == -1) return;`，无 found 具名局部（P0：ORIG 无槽往返）。
    // 旧形 `bool found=false; ... if(!found) return;` 在 m_userCount<=1 时也早退
    // （不重置）—— 语义真 bug，已修。
    if (m_userCount > 1)
    {
        PacketGuard packet;
        packet.put_header(0, 0x160);
        unsigned int bestIdx = 0xffffffff;
        int bestSlot = -1;
        for (int i = 0; i <= 5; ++i)
        {
            // b28d-solo-WarRoom [P223]：ORIG 0x86c09c7/0x86c09d9 两个独立 je 各落
            // nop 中继岛（0x86c0a05 nop;jmp / 0x86c0a08 nop;fall），第三个守卫
            // `ja 0x86c0a09` 无岛直落 —— 负条件 goto + 尾标签串唯一复现形
            // （&& 短路链被 4.4 jump-thread，少 2 岛）。
            if (m_slots[i] == 0)
                goto CHC_NEXT1;
            if (m_hostVotes[i] == -1)
                goto CHC_NEXT2;
            if ((unsigned int)m_hostVotes[i] <= bestIdx)
            {
                bestIdx = (unsigned int)m_hostVotes[i];
                bestSlot = i;
            }
CHC_NEXT1:
            ;
CHC_NEXT2:
            ;
        }
        if (bestSlot == -1)
            return;
        packet.put_byte(bestSlot);
        packet.finalize(true);
        SendToRoom(packet);
    }
    for (int i = 0; i <= 5; ++i)
        m_hostVotes[i] = -1;
    m_userCount = 0;
}