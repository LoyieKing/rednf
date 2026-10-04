// [b28d-lane2] verdict=DIFF ndiff=84（ORIG 0x084e5c26）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=83（ORIG 0x84e5c26）：N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_Monitor_MegaphoneMsgHyperLink::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [P340 一试 2026-09-11] 指针局部改整型承载：ORIG pPkt 落 -0x10（整型分区、贴槽基），
    //   4.4 对指针局部另立分区（落 -0x14）⇒ 整帧槽平移 4（全部 34 块差异同根因）。
    unsigned int pPktAddr = reinterpret_cast<unsigned int>(data);
    // [R14a05 2026-09-12] ORIG 0x84e5c2e/0x84e5d47 实测：pPkt 落 -0x10（深）、i 落 -0xc（浅）
    //   ⇒ 原码声明序 = pPkt 在前、i 在后（4.4 同向「先声明更深」）；旧形两槽互换 → 交换。
    int i;
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
    packet.put_int((int)strlen(MPKT->m_name));
    packet.put_str(MPKT->m_name, (int)strlen(MPKT->m_name));
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
        // [R14a05 2026-09-12 负结论] P161 嵌套组件（Rec@+0x20 步长 0x68 + m_data@+0xe）
        //   已试并回滚：4.4 仍先载基址（多 1 条 mov），差异量与平铺形相同、零收益。
        // [R17a13] ORIG 0x084e5d59 `add -0x10(%ebp),%eax` = 从 pPkt 具名槽取基址；
        //   旧形直接写形参 data（0x10(%ebp)）⇒ 4.4 前向替换成帧参直读（P358），
        //   与 ORIG 槽读不同形。改回经 pPktAddr（MPKT 同源）表达。
        packet.put_binary(
            MPKT->m_items[i].m_data, 0x68);
    }
    packet.finalize(true);

    // [R14a05 2026-09-12] ORIG 0x84e5da9 段四元组（§16.1）：
    //   O-taken `test %al,%al; je 0x84e5dd6` = send_all 块；O-fall = send_server_group 块。
    //   旧形负极性（`!f()` + xor $1）致两臂互换 + 多一条 xor ⇒ 改正极性正向 if，
    //   两臂块内容逐条等同（仅目标块序归位）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        // [R13a10] ORIG 0x84e5db2 段：`movzbl 0xa(%eax),%eax; movsbl %al,%ebx`
        // 无槽往返（我方旧形多 `mov %al,-0x9(%ebp)` + 槽读共 2 条）⇒ 实参内联。
        G_GameWorld()->send_server_group(packet, (char)MPKT->m_a);
    }
    else
    {
        G_GameWorld()->send_all(packet);
    }
#undef MPKT
    return 0;
}