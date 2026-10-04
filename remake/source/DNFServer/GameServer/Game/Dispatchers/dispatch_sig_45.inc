// ORIG 0x0821e4b6（报告 Dispatcher_Overflow_Info/dispatch_sig.md）：
// Packet_Overflow_Statistic_Add(0xc43,0xd) = PacketHeader(10B)+0xa uchar+0xb ushort。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821e4b6 orig/ours=55/59 ndiff-blocks=11
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | lea    -0x15(%ebp),%eax
int Dispatcher_Overflow_Info::dispatch_sig(CUser* user, PacketBuf& packet)
{
    Packet_Overflow_Statistic_Add pkt;
    // ORIG 段直证：两个读入目标都经 `lea -0x15(%ebp),%eax; add $0xa/0xb,%eax` 内联
    // 取址（无引用槽）；原具名引用在 -0x18/-0x14 各落一槽，帧 0x24→0x38。
    if (!packet.get_byte(pkt.m_kind))
    {
        return LineFunc(0xd52f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(pkt.m_val))
    {
        return LineFunc(0xd530,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [DispA2 P0/P183] ORIG 0x0821e55a/0x0821e567/0x0821e574：GetServerGroup 的回值
    //   直入 GetServerProxy 的 0x4(%esp)；GetServerProxy 回值直作 SendPacket 的 receiver；
    //   ORIG 无 group(-0x10)/proxy(-0xc) 具名槽（旧帧 0x38 vs ORIG 0x24 的主因）。
    (*GlobalData::s_statistic_proxy_mgr)
        .GetServerProxy(user->GetServerGroup())
        ->SendPacket((char*)&pkt, 0xd);
    return 0;
}