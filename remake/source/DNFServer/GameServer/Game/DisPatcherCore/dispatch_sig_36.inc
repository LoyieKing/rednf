// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 345（ORIG 228 / ours 233），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
int DisPatcher_UdpCharacteristic::dispatch_sig(CUser* user, PacketBuf& packet)

{
    unsigned short local_e = 0;
    unsigned char local_f = 0;
    if (packet.get_short(local_e) != true)
    {
        return LineFunc(0xce12,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_byte(local_f) != true)
    {
        return LineFunc(0xce14,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [Lane38 2026-09-10] 判据①/⑧ 块布局：ORIG 0x0821ac7c 分派为 **switch 比较链**
    // `movzwl -0xa(%ebp),%eax; movzwl %ax,%eax; cmp $1,%eax; je +0x193;
    //  cmp $2,%eax; je +0x2d3; test %eax,%eax; jne +0x3aa`（fall-through = case 0，
    // default 在 case2 之后）；if-else 链的 4.4 O0 形是就近 `jne` + 各块 fall-through
    // （我方旧形态 jcc 目标差 −106/+134/+133）。双 `movzwl` 提升亦为 switch 表达式
    // int 提升直证（if 比较为 `cmp $1,%ax` 16 位）。语义等价改写。
    switch (local_e)
    {
    case 0:
    {
        Packet_Party_Result_Statistic local_1d;
        // [G5-SigA] ORIG 0x0821aca1 `lea -0x19(%ebp),%eax; add $0xa,%eax` + 0x0821ace6
        // `...; add $0xc,%eax`：两个 get_short 直接写结构成员本身（+0xa/+0xc），
        // 随后 0x0821ad2b `movzwl -0xd(%ebp),%edx; movzwl -0xf(%ebp),%eax;
        // lea (%edx,%eax,1),%eax; mov %ax,-0xd(%ebp)` 才做 total += success。
        // 原写法用两个具名 2B 临时量中转（P0：ORIG 无该槽），改为直传成员地址。
        if (packet.get_short(*(unsigned short*)&local_1d.m_successPartyTry) != true)
        {
            return LineFunc(0xce1d,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (packet.get_short(*(unsigned short*)&local_1d.m_totalPartyTry) != true)
        {
            return LineFunc(0xce1e,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        local_1d.m_totalPartyTry = static_cast<unsigned short>(
            local_1d.m_totalPartyTry + local_1d.m_successPartyTry);
        // [SigB] P0/P183：ORIG 0x0821ad0x 无这两个具名槽——`user->GetServerGroup()`
        // 结果直推实参、proxy 结果留 %eax，&local_1d 保活于 %ebx（ORIG `mov %ebx,0x4(%esp)`）。
        // 我方原写两个具名局部使帧深 0x10（-0x18/-0x14 槽）。改单表达式链。
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->SendPacket(reinterpret_cast<char*>(&local_1d), 0xe);
        break;
    }
    case 1:
    {
        unsigned short local_26 = 0;
        unsigned short local_24 = 0;
        if (packet.get_short(local_26) != true)
        {
            return LineFunc(0xce30,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (packet.get_short(local_24) != true)
        {
            return LineFunc(0xce31,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (local_f == '\0')
        {
            partyPingStat(user, local_26, local_24);
        }
        else if (local_f == '\x01')
        {
            pvpPingStat(user, local_26, local_24);
        }
        // [DPC-SigC 2026-09-11] P324 叶块书写序：ORIG 0x0821ae9x 段为
        // `movzbl local_f; cmp $0x2,%al; jne <LineFunc 块>`（即 then=fairPvpPingStat
        // 走 fall-through，LineFunc(0xce44) 块位于其后，LCS O150 `jne` vs
        // 我方 `je`+inline 块前置，双方各 6+1 条同量但块序相反）。原嵌套
        // `if (local_f != 2) return LineFunc;` 使 then=LineFunc 前置。改为
        // `else if (local_f == 2) fairPvpPingStat; else return LineFunc;` 复现
        // ORIG 的 fall-through 方向。语义等价（三分支互斥）。预期 2 差异块→0，
        // 指令数 228 不变。
        else if (local_f == '\x02')
        {
            fairPvpPingStat(user, local_26, local_24);
        }
        else
        {
            return LineFunc(0xce44,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        break;
    }
    case 2:
    {
        Packet_Abnormal_Exit_Statistic local_30;
        // [G5-SigA] 同 case0：ORIG 0x0821aebe `lea -0x2c(%ebp),%eax; add $0xa,%eax`
        // 与 0x0821af03 `...; add $0xc,%eax` 直写成员，加法在 0x0821af48 后进行。
        if (packet.get_short(*(unsigned short*)&local_30.m_successDungeonClear) != true)
        {
            return LineFunc(0xce68,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (packet.get_short(*(unsigned short*)&local_30.m_totalDungeonClear) != true)
        {
            return LineFunc(0xce69,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        local_30.m_totalDungeonClear = static_cast<unsigned short>(
            local_30.m_totalDungeonClear + local_30.m_successDungeonClear);
        // [SigB] 同 case0：删具名 uVar2/pCVar3（ORIG -0x10/-0xc 无对应槽写入）。
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->SendPacket(reinterpret_cast<char*>(&local_30), 0xe);
        break;
    }
    default:
        // ORIG 0x0821af8a（+0x3aa，case2 块之后）：非 0/1/2 值 → LineFunc(0xce76)。
        return LineFunc(0xce76,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
