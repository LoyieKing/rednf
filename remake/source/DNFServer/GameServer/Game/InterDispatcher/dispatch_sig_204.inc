int Inter_GuildCargo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2A-PC 本轮负结论 N，ndiff=11] 残留两类：
    //   (a) `mov <S>,%edx` 较 ORIG 早/晚 1 条（块序抖动）；
    //   (b) ORIG 三段式 `add $0x10,%eax; mov 0x3(%eax),%eax`、`add $0x10; add <S>; add $0x2`
    //       vs 本方折叠形 `mov 0x13(%eax),%eax`、`lea (%edx,%eax,1); add $0x12`
    //       —— 同一表达式的 gcc 指令选择差（本件基址已是 pPkt 具名指针）。
    //   两者皆无源码级单点直证，留 N。
    Inter_GuildCargoPktView* pPkt = reinterpret_cast<Inter_GuildCargoPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    // ORIG：同 ApplyTempPowerSide。
    if (user->get_charac_no(-1) != pPkt->m_e)
        return 0;

    {   // ORIG 0x084dd2df：GuildCargoResult(reason=1, a=msg+0x18ee, b=capacity=msg+0x18ea)
        //   （esp+4=1、esp+8=0x18ee、esp+0xc=0x18ea——reason 为常量 1，勿与字段错位）
        // ORIG 0x084dd2df 前 `mov 0xc,%ecx; add $0x79700,%ecx; mov %ecx,(%esp)`：
        // hist 免槽即算直落（具名 hist 槽会顶深 pPkt 至 -0x18，ORIG -0x14）。
        user->m_historyLog.GuildCargoResult(
            (ENUM_GUILD_CARGO_HISTORY_REASON)1,
            (int)pPkt->m_18ee,
            (int)pPkt->m_18ea);
    }

    // ORIG 0x084dd30a `movzbl 0x18ee; cmp $0xc1,%al; je +0xd0`：字段 unsigned char
    // 与 0xc1 直接字节比较（原 (char)0xc1 恒真不等致 4.4 折叠整个成功路径 -106 行，
    // 技巧46 同款语义 bug 已纠正）。
    if (pPkt->m_18ee != 0xc1)
    {   // 失败：SendCmdErrorPacket(0x109, errCode)
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x109,
            pPkt->m_18ee);
        return 0;
    }

    user->SetGuildCargoCapacity(
        pPkt->m_18ea);

    int index = 0;                                     // local_1c
    int count = 0;                                     // local_14
    PacketGuard packet;
    packet.put_header(1, 0x109);
    packet.put_byte(1);
    packet.put_int((int)pPkt->m_18ea);
    index = packet.get_index();
    packet.put_short(count);
    for (int i = 0; i <= 0x77; ++i)                    // cmpl $0x77; setle
    {
        // ORIG 0x084dd3dc..：entry 数组（stride 0x35）+ payload 嵌套成员 ⇒
        // 两段地址 `add $0x10` + disp；put_binary 地址独立重算（P48 无 CSE）。
        // entries 具名指针占槽顶深 pPkt（技巧36），改 rvalue cast 免槽。
        if (G2_GC_PAYLOAD()->m_flag != 0)
        {
            packet.put_short((int)(short)i);
            packet.put_binary(
                reinterpret_cast<char*>(&G2_GC_PAYLOAD()->m_blob0),
                0x35);
            count = count + 1;
        }
    }
    packet.put_short(index, count);                    // put_shortERii 回填总数
    packet.finalize(true);
    user->Send(packet);
    return 0;
}