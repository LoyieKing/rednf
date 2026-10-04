// [b28d-lane2] verdict=DIFF ndiff=115（ORIG 0x084dd4b2）：寄存器中转(N18a/N7)+混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=49（ORIG 0x84dd4b2）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→2（137 vs 135，TU 121/1331/5/133 不回退）：真修三处——
//   ① countIdx/putCount 提到 PacketGuard 之前且 countIdx 先声明（复现 ORIG `movl $0,-0x18; movl $0,-0x10` 序）；
//   ② `put_short((short)i)` 复现 ORIG `cwtl` 截断；③ m_e int→unsigned int 触发比较点中转（P211/P265 档③）。
//   残余=data 形参 vs ORIG -0x14 槽基址 + 守卫块序（P30/N4）。
int Inter_NoticeGuildCargo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2A-PC 本轮核销 + 负结论 N，ndiff=8]
    //   · 线索核销：ORIG 该函数内 put_binary 调用点为 0x0816f45e（nm 反查
    //     `_ZN18InterfacePacketBuf10put_binaryEPKci`）⇒ ORIG 用的是 **char const\*** 重载，
    //     与本方 `reinterpret_cast<const …>(pPkt)` 实参一致；任务书「重载选错」线索不成立，
    //     无需把指针改成非 const（改为 char\* 反而会错选 _ZN…put_binaryEPci）。
    //   · 残留：`mov <S>,%edx` 块序 ±1；ORIG 三段式 `add $0x10; add <S>; add $0x2`
    //     vs 本方 `lea (%edx,%eax,1); add $0x12`（同 204 的指令选择差）⇒ 留 N。
    Inter_NoticeGuildCargoPktView* pPkt = reinterpret_cast<Inter_NoticeGuildCargoPktView*>(data);
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle; je 返回
        return 0;

    if (user->get_charac_no(-1) !=
        pPkt->m_characNo)         // msg+0x0e 角色号校验
        return 0;

    user->m_historyLog.GuildCargoResult(
        (ENUM_GUILD_CARGO_HISTORY_REASON)2, 0,
                           pPkt->m_capacity);
    user->SetGuildCargoCapacity(                     // msg+0x18ea 容量
        pPkt->m_capacity);

    int countIdx = 0;                                // local_1c/asm -0x18(%ebp)
    int putCount = 0;                                // local_14/asm -0x10(%ebp)
    PacketGuard packet;
    packet.put_header(0, 0xed);
    packet.put_int(pPkt->m_capacity);
    countIdx = packet.get_index();                   // 回填位
    packet.put_short(putCount);
    for (int i = 0; i <= 0x77; ++i)                  // asm cmpl $0x77, setle
    {
        // ORIG 循环 `mov -0x14(%ebp),%edx` 基址取自具名槽（非 0x10(%ebp) 形参槽）⇒
        // 元素访问经同一具名 base（pPkt）而非形参，避免第二指针槽（P89/P0）。
        // 残：ORIG `mov -0xc(%ebp),%eax; mov -0x14(%ebp),%edx; imul; lea`（先 i 后 base）
        // vs 4.4 恒 base-first（`i + ptr` 形实测同 base-first）⇒ P218/P30 装载序封顶。
        if (reinterpret_cast<const Inter_NoticeGuildCargoTail*>(
                (reinterpret_cast<const Inter_NoticeGuildCargoEntry*>(pPkt) + i)
                    ->m_body.m_bytes)->m_3 != 0)
        {
            packet.put_short((short)i);              // ORIG `cwtl`（源码 (short) 截断）
            packet.put_binary(
                (reinterpret_cast<Inter_NoticeGuildCargoEntry*>(pPkt) + i)
                    ->m_body.m_bytes + 2, 0x35);
            ++putCount;
        }
    }
    packet.put_short(countIdx, putCount);            // put_short(int&,int) 回填
    packet.finalize(true);
    user->Send(packet);
    return 0;
}