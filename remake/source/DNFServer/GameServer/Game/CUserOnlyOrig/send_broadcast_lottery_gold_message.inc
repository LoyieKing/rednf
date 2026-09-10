void CUser::send_broadcast_lottery_gold_message(unsigned long itemIdx, int count)
{
    CItem* def = G_CDataManager()->find_item((int)itemIdx);
    if (def == 0) return;
    if (!def->is_stackable()) return;
    // def 已过 is_stackable 守卫，动态类型 CStackableItem；+0x340 m_int340
    // [b28d-22 修法 P28] ORIG 0x0868d32x `mov -0x10(%ebp),%eax; mov %eax,-0xc(%ebp);
    // mov -0xc(%ebp),%eax; mov 0x340(%eax),%eax` ⇒ 原码有具名 CStackableItem* 局部
    // （槽 -0xc），内联 cast 形缺该槽且帧少 4。
    CStackableItem* stack = (CStackableItem*)def;
    // [b28d-solo-CUserOnlyOrig P63/N] 三散布早退 4.4 各产 jmp+nop 中继岛（4 vs ORIG 2）；
    // 正向包裹合并形实测反产 bool 物化岛（mov$1/mov$0）且助记符序列 DIFF ⇒ 回退独立
    // 守卫（原状 ndiff=2），岛数差按 P63/N121 封顶。
    // [b28d-solo-CUserOnlyOrig P63/P223] ORIG 后两条件 `jle/jg 0868d323`（直落 epilogue，
    // 无岛），仅前两早退各带岛（d31f/d322）。early-return 合并形在 4.4 恒各生岛（4 vs 2）
    // ⇒ 改正向包裹：两条件失败跳 body 之后（epilogue）。
    if (stack->m_int340 > 0 && stack->m_int340 <= count)
    {
        PacketGuard packet;
        packet.put_header(0, 0x56);
        packet.put_byte(9);
        packet.put_short(get_unique_id() & 0xffff);
        packet.put_int(itemIdx);
        packet.put_int(count);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
    }
