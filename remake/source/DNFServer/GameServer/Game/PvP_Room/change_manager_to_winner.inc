// [b28d-7 N-P223] ORIG 0x085d7df0：双守卫 je/jne 分落 +0x15c/+0x15d 两独立 landing，4.4 合并共享出口 +1 jmp；continue 形与 goto 标签串形实测均 +1（P223 单源岛合并负例）⇒ 封顶。
// [b28d-solo-PvP_Room] N-P223: ORIG 0x085d7d54 双守卫分落 +0x15c/+0x15d 两独立 landing，4.4 合并共享出口
int PvP_Room::change_manager_to_winner()
{
    // ORIG 0x085d7d54（81；本侧 83，+2=N 落点：ORIG seat==0 skip 的 je 与
    // is_last_win!=true skip 的 jne 分落 +0x15c/+0x15d 两独立 landing（其间
    // 1 条 nop/重叠指令布局产物），4.4 合并共享出口+中继 jmp。bVar 常量岛/
    // !=1→xor 物化/嵌套实参消槽三形已应用，141→83。）
    // [batch-19 N 封顶 ndiff=1] 再实测三形均不优于双 continue：
    // ① 合并 `if (seat!=0 && is_last_win(i)==true)` → 84 insn（丢 `xor$1`
    //   物化且中继仍在）；② 纯嵌套 `if(seat){if(==true){body}}` → 79 insn
    //   （极性翻 test/je，丢 ORIG `xor$1;jne`）；③ `!is_last_win(i)` 替
    //   `!= true` → 83 insn 逐条同（xor$1 保留），无增益。
    if (m_seats[7] == 0)
    {
        return 0;
    }
    if (G_GameWorld()->GetChannelType() == 0xf)
    {
        return 1;
    }
    if (G_GameWorld()->GetChannelType() == 7 ||
        G_GameWorld()->IsSchoolPvPChannel() != 0)
    {
        return 1;
    }
    if (is_last_win(m_managerSeat))
    {
        return 1;
    }
    // [Lane10] N108 封顶（本件）：ORIG 0x085d7e9f(seat==0) 落 +0xed(循环增量)、
    // 0x085d7eb8(is_last_win!=true) 落 +0xec(nop) —— 双守卫分落两个相邻但不同的
    // landing。本轮实测四形均恒多 1 条中继 jmp（4.4 jump-thread 共享出口）：
    // ① 双 goto 同一标签（for）；② while + 两独立尾标签（NOP 在前/INC 在后）；
    // ③ continue + goto 单标签；④ 空 then + else 嵌套。语义全等（两落点都到
    // addl），P167 CLEAR ⇒ 按 N108 记封顶，恢复最简形。
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        if (P_SEATS(local_10) == 0)
            goto CMW_NEXT;
        if (is_last_win(local_10) != true)
            goto CMW_NEXT;
        m_seats[7] = P_SEATS(local_10);
        m_managerSeat = local_10;
        return 1;
    CMW_NEXT:
        ;
    }
    return 0;
}