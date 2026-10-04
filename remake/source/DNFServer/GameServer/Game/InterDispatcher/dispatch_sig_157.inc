// [b28d-lane2] verdict=DIFF ndiff=132（ORIG 0x084d8096）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-6。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=94（ORIG 0x84d8096）：N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_PvPChannelInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;

    // [L9-G PATCH#2] P-L11c：ORIG 入口 `cmpl $0x0,0xc(%ebp); jne <+0x18>; mov $0x0,%ebx; jmp <尾>`，
    //   `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`（pPkt 物化）落在 jne 之后；旧形提前物化。
    //   探针 /tmp/L9/G/pg2.cpp g1（前置）/g2（后置）：g2 = ORIG 形。
    //   ⚠ 本波实测**零变化**（fn_diff 95→95，指令数 142 vs ORIG 133 不变）—— 与本函数残差
    //   主体（整帧槽平移 + 循环体 base 形）耦合；保留此形（探针直证正确）不再单独回滚。
    // [P340 一试 2026-09-11] 指针局部改整型承载：ORIG pPkt 落 -0x10（与 i@-0xc 相邻的整型分区），
    //   4.4 对指针局部另立分区（落 -0x14）⇒ 整帧差 4。整型承载同时令地址算术走整型链（P319）。
    unsigned int pPktAddr = reinterpret_cast<unsigned int>(data);

    // P0/P183：ORIG 无 msg 具名槽（各用点重新 `mov 0x10(%ebp),%eax`），且地址按
    //   char* 基址 + (i+1)*0x10 + off 计算（形如 `mov 0x8(%edx,%eax,1)`），不是 int* 指针算术。
    if (user->getCurCharacR() == 0 ||                // asm 双失败归并 true
        user->getCurCharacNo() !=
            reinterpret_cast<Inter_PvPChannelInfoPktView*>(pPktAddr)->m_a)  // msg+0x0a 角色号
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0xc6);
    packet.put_byte(1);
    packet.put_int(reinterpret_cast<Inter_PvPChannelInfoPktView*>(pPktAddr)->m_12);  // msg+0x12
    // [P0 2026-09-11 二试] ORIG 无 channelCnt 具名槽：put_byte 实参内联 + 循环条件每轮重读
    //   m_17（`mov -0x10(%ebp),%eax; movzbl 0x17(%eax)`）⇒ 均为表达式直用（pPktAddr 槽独占 -0x14 前的 -0x10 位）。
    packet.put_byte(static_cast<int>(reinterpret_cast<Inter_PvPChannelInfoPktView*>(pPktAddr)->m_17));
    for (int i = 0; i < static_cast<int>(reinterpret_cast<Inter_PvPChannelInfoPktView*>(pPktAddr)->m_17); ++i)
    {
        // [L9-G PATCH#3] 循环内基址走具名槽（ORIG `mov -0x10(%ebp),%eax` + SIB `mov 0x8(%edx,%eax,1)`）。
        packet.put_int(reinterpret_cast<Inter_PvPChannelInfoPktView*>(pPktAddr)->m_ch[i].m_a);
        packet.put_int(reinterpret_cast<Inter_PvPChannelInfoPktView*>(pPktAddr)->m_ch[i].m_b);
        if (reinterpret_cast<Inter_PvPChannelInfoPktView*>(pPktAddr)->m_ch[i].m_d == 2)
            reinterpret_cast<Inter_PvPChannelInfoPktView*>(pPktAddr)->m_ch[i].m_c = 4;
        packet.put_int(reinterpret_cast<Inter_PvPChannelInfoPktView*>(pPktAddr)->m_ch[i].m_c);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}