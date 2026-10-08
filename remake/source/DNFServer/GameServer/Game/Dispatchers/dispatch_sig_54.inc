// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08223de0 orig/ours=416/398 ndiff-blocks=66
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x15d(%ebp),%eax | movb   $0x0,-0x11(%ebp) | lea    -0x15d(%ebp),%eax
int Dispatcher_Lag_Statistics::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08223de0 [0x08223de0, 0x08224499]
    Packet_Stat_Lag_Statistics local_161;                    // ctor ORIG 0x0822c144（弱内联）
    // [Lane1Dispatchers 2026-09-10] P183：ORIG 无具名 base 指针局部（每处重发
    //   `lea`，0x08223de0 段实证）⇒ 宏内联（零局部），函数尾 `#undef base`。
#define base (reinterpret_cast<unsigned char*>(&local_161))
    // [DNF-FIX 2026-10-08 c31] ORIG 0x08223e10 get_byte(base+0xa)、0x08223e47/0x0822440b 读 -0x153=base+0xa：
    //   计数直接写入包体 m_flagA（无独立局部、无清零）。
    unsigned char& count = reinterpret_cast<unsigned char&>(local_161.m_flagA);
    if (!packet.get_byte(count))
    {
        return LineFunc(0xde9f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (8 <= count)
    {
        return LineFunc(0xdea1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char idx = 0;                                   // -0x11（循环内局部）
    for (int i = 0; i < (int)(unsigned int)count; ++i)
    {
        if (!packet.get_byte(idx))
        {
            return LineFunc(0xdea7,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (7 < idx)
        {
            return LineFunc(0xdeab,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_int(local_161.m_modules[idx].m_a))
        {
            return LineFunc(0xdead,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_int(local_161.m_modules[idx].m_b))
        {
            return LineFunc(0xdeae,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_short(local_161.m_modules[idx].m_c))
        {
            return LineFunc(0xdeaf,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (idx == 1)
        {
            // [DNF-FIX 2026-10-08 c31] ORIG 0x08224036 get_byte(base+0x5b)、0x0822406d/0x082243ef 读 -0x102=base+0x5b。
            unsigned char& count2 = reinterpret_cast<unsigned char&>(local_161.m_flagB);
            if (!packet.get_byte(count2))
            {
                return LineFunc(0xdeb4,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            if (10 < count2)
            {
                return LineFunc(0xdeb6,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            for (int j = 0; j < (int)(unsigned int)count2; ++j)
            {
                if (!packet.get_short(local_161.m_dungeons[j].m_w0))
                {
                    return LineFunc(0xdebc,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_byte(local_161.m_dungeons[j].m_b2))
                {
                    return LineFunc(0xdebd,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_int(local_161.m_dungeons[j].m_i3))
                {
                    return LineFunc(0xdebe,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_int(local_161.m_dungeons[j].m_i7))
                {
                    return LineFunc(0xdebf,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_short(local_161.m_dungeons[j].m_wb))
                {
                    return LineFunc(0xdec0,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_byte(local_161.m_dungeons[j].m_bd))
                {
                    return LineFunc(0xdec1,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_int(local_161.m_dungeons[j].m_ie))
                {
                    return LineFunc(0xdec2,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_int(local_161.m_dungeons[j].m_i12))
                {
                    return LineFunc(0xdec3,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_short(local_161.m_dungeons[j].m_w16))
                {
                    return LineFunc(0xdec4,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
            }
        }
    }
    // [DispA2 P0/P183] ORIG 段 `call <CServerProxyMgr<CStatisticServerProxy>::GetServerProxy>`
    //   之后紧接 `movl $0x14c,0x8(%esp); mov %ebx,0x4(%esp); mov %eax,(%esp)`——回值
    //   直作 SendPacket 的 receiver，无 proxy(-0x14) 具名槽。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(base), 0x14c);
    return 0;
}