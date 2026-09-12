// [b28d-lane2] verdict=DIFF ndiff=84（ORIG 0x084e5c26）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=83（ORIG 0x84e5c26）：N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_Monitor_MegaphoneMsgHyperLink::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [P340 一试 2026-09-11] 指针局部改整型承载：ORIG pPkt 落 -0x10（整型分区、贴槽基），
    //   4.4 对指针局部另立分区（落 -0x14）⇒ 整帧槽平移 4（全部 34 块差异同根因）。
    int i;                                    // [P339] 头部声明：先声明深（ORIG i 深于 pPkt）
    unsigned int pPktAddr = reinterpret_cast<unsigned int>(data);
#define MPKT reinterpret_cast<Inter_Monitor_MegaphoneMsgHyperLinkPktView*>(pPktAddr)
    PacketGuard packet;
    packet.put_header(0, 0x174);
    packet.put_byte(
        (int)MPKT->m_e);
    packet.put_byte(
        (int)MPKT->m_b);
    packet.put_short(
        (int)(unsigned short)MPKT->m_c);
    // ORIG 0x84e5cae/0x84e5ccb：strlen 双调（put_int/put_str 各算一次，O0 不 CSE）
    packet.put_int((int)strlen(MPKT->m_f));
    packet.put_str(MPKT->m_f, (int)strlen(MPKT->m_f));
    // [K3 2026-09-11] P0：ORIG 两处 `movzbl 0x166(%eax),%eax; movzbl %al,%eax` 重读
    //   （无 linkLen 具名槽）⇒ 内联。
    packet.put_int((int)(unsigned int)MPKT->m_166);
    packet.put_str(MPKT->m_167, (int)(unsigned int)MPKT->m_166);
    packet.put_byte(
        (int)MPKT->m_2d);
    for (i = 0;
         i < (int)(unsigned int)MPKT->m_2d;
         ++i)
    {
        packet.put_binary(data + i * 0x68 + 0x2e, 0x68);
    }
    packet.finalize(true);

    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        G_GameWorld()->send_all(packet);
    }
    else
    {
        // [R13a10] ORIG 0x84e5db2 段：`movzbl 0xa(%eax),%eax; movsbl %al,%ebx`
        // 无槽往返（我方旧形多 `mov %al,-0x9(%ebp)` + 槽读共 2 条）⇒ 实参内联。
        G_GameWorld()->send_server_group(packet, (char)MPKT->m_a);
    }
#undef MPKT
    return 0;
}