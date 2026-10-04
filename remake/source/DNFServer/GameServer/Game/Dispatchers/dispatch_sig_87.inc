// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0821efec orig/ours=361/351 ndiff-blocks=58
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x820(%ebp),%eax | movw   $0x0,-0x16(%ebp) | movl   $0x0,-0x14(%ebp)
int Dispatcher_LoadingTimeReport::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821efec [0x0821efec, 0x0821f611]
    Packet_Loading_Time_Report_Statistics local_824;   // ctor ORIG 0x0822c040（弱内联）
    // 布局无头文件证据，按 ORIG 裸偏移访问（对象栈帧 -0x820 起，payload 尺寸 0x806）。
    // [Lane1Dispatchers 2026-09-10] P183：ORIG **无**具名 base 指针局部——每处访问都
    //   重发 `lea -0x820(%ebp),%eax`（ORIG 0x0821efec 段实证）；我方具名指针占 4B 槽
    //   使整帧 +8、对象落 -0x828（ORIG -0x820）并连带全部槽号错位。改为宏内联
    //   （等价全表达式展开、零具名局部），函数尾 `#undef base`。
#define base (reinterpret_cast<unsigned char*>(&local_824))
    unsigned short idx = 0;                            // -0x16
    int i = 0;                                         // -0x14（循环计数）
    while (i <= 8)
    {
        if (!packet.get_short(idx))
        {
            return LineFunc(0xd7fe,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (8 < idx)
        {
            return LineFunc(0xd800,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (idx < 7)
        {
            if (!packet.get_int(local_824.m_times[idx]))
            {
                return LineFunc(0xd806,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
        }
        local_824.m_accId = user->get_acc_id();
        if (idx == 7)
        {
            short count = 0;                           // -0x18
            if (!packet.get_short(count))
            {
                return LineFunc(0xd80c,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            if (99 < count)
            {
                return LineFunc(0xd80d,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            local_824.m_countA = count;
            for (int j = 0; j < count; ++j)            // -0x10；ORIG cwtl 后有符号 setg
            {
                if (!packet.get_short(local_824.m_rowsA[j].m_a))
                {
                    return LineFunc(0xd811,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_byte(local_824.m_rowsA[j].m_b))
                {
                    return LineFunc(0xd812,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_short(local_824.m_rowsA[j].m_c))
                {
                    return LineFunc(0xd813,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_short(local_824.m_rowsA[j + 1].m_link0))
                {
                    return LineFunc(0xd814,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_short(local_824.m_rowsA[j + 1].m_link1))
                {
                    return LineFunc(0xd815,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
            }
            local_824.m_times[7] = 0;
        }
        if (idx == 8)
        {
            short count = 0;                           // -0x1a
            if (!packet.get_short(count))
            {
                return LineFunc(0xd81c,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            if (99 < count)
            {
                return LineFunc(0xd81d,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            local_824.m_countB = count;
            for (int j = 0; j < count; ++j)            // -0xc；ORIG cwtl 后有符号 setg
            {
                if (!packet.get_short(local_824.m_rowsB[j].m_a))
                {
                    return LineFunc(0xd821,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_byte(local_824.m_rowsB[j].m_b))
                {
                    return LineFunc(0xd822,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_int(local_824.m_rowsB[j].m_c))
                {
                    return LineFunc(0xd823,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_int(local_824.m_rowsB[j].m_d))
                {
                    return LineFunc(0xd824,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
            }
            local_824.m_times[8] = 0;
        }
        ++i;
    }
    CStatisticServerProxy* proxy = GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendPacket(reinterpret_cast<char*>(base), 0x806);
    return 0;
}