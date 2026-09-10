// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::Disconnect（原 TU 第 1582 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-solo-PvP_Room] N-P223: ORIG 0x085dd754 je 落 0x085dd809 nop 岛、jne 落 addl；goto/continue 四形实测 +2
int PvP_Room::Disconnect(CUser* user)
{
    // [b28d-7 AE] ORIG 0x085dd774 je 落 nop 岛（0x085dd809）、0x085dd789 jne
    // 落循环尾 addl（0x085dd80a）；双 continue + && 短路形经 446h46 路由即逐条
    // 全等（goto 标签串实测反劣 +1 jmp，已回退）。命中座后无条件 return 1。
    // [b28d-solo-PvP_Room] 复验：ORIG 0x085dd809 nop 占位岛（je 目标）与
    // 0x085dd80a addl（jne 目标）为 4.1 短路 && 的双落点；P223 否定 goto 尾标签
    // 形实测 +2（nop;jmp;nop 中继），共享标签/双 continue/while+goto 四形均 +2
    // ⇒ 单源岛合并负例封顶；if 包裹形与 ORIG 仅差该 nop（60 vs 61）。
    for (int i = 0; i < 8; i = i + 1)
    {
        if ((P_SEATS(i) != 0) && (P_SEATS(i) == user))
        {
            ((IMatch*)m_matching)->OnWalkOut(this, i);
            if (!WalkOut(P_SEATS(i), i, ENUM_SEAT_STATE_0xfe,
                         (ENUM_WALKOUT_TYPE)3))
            {
                ClearPvPRoom(user);
            }
            return 1;
        }
    }
    return 0;
}
