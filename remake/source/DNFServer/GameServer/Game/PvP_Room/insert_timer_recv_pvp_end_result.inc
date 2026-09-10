// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::insert_timer_recv_pvp_end_result（原 TU 第 2897 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-solo-PvP_Room] P0 具名局部: ORIG key->edi + m_index 落 -0x1c 槽（P0 槽往返证据），语句表达式形已复现该分配；残余 idx 槽写时机（4.1 延后物化）封顶
// [b28d-solo-PvP_Room] 本轮复测（N7/N18a 封顶）：ORIG key→%edi + m_index 落 -0x1c；
// 4.4 恒把首个 callee 结果（key）spill 到 -0x2c 并把 m_index 经 -0x1c 升格 %edi
// （角色互换）。五形实测：内联双参 43 条（key 落 -0x1c、m_index→edi，角色仍反）、
// m_index 语句表达式 + 内联 key 45 条（取最优，槽位与 ORIG 同集合）、P197② 具名
// key 47 条、裸语句表达式 key 45 条、register 提示 46 条 ⇒ 不可翻，封顶。
int PvP_Room::insert_timer_recv_pvp_end_result()
{
    // [b28d-solo-PvP_Room P0/P226] ORIG 0x085dc23c：gen_timer_key→%edi、m_index
    // 经 -0x1c 槽 store/reload（P0 判据：槽往返 ⇒ 原码有具名局部/语句表达式临时）。
    // 语句表达式 `({ int idx = m_index; idx; })` 复现 key→%edi + m_index 落 -0x1c
    // 槽；残余仅 idx 槽写时机（4.1 延后到 timeout 求值后，4.4 提前到语句头）。
    // [Lane10] 本轮再试 P226 纯语句表达式包 key（`({ gen_timer_key(...); })`）⇒
    // 仍 spill 到 -0x2c 槽（45 条不变），与历史 5 形一致 ⇒ N7 角色互换封顶
    // （ORIG 0x085dc258 `mov %eax,%edi` = 4.1 把首个 callee 结果留 callee-saved）。
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)1, ({ int idx = m_index; idx; }), (TIMER_MESSAGE)0x3d,
        ((IMatch*)m_matching)->GetEndPvPResultTimeout(),
        (int)gen_timer_key((TIMER_MESSAGE)0x3d), 0);
    return 1;
}
