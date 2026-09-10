// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::check_recv_pvp_rank（原 TU 第 2913 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

int PvP_Room::check_recv_pvp_rank()
{
    // ORIG 0x085dc3ca：bool 槽 -0xd（movzbl 返回）；user 直落 %ebx 跨调用；
    // IsRecv(char) 的 ! 以 xor $1/test/je 短路，不物化 0/1。
    unsigned char changed = 0;
    for (int i = 0; i < 8; i = i + 1)
    {
        // ORIG 085dc3ca：4.1 双空 skip 块岛 085dc455(nop;jmp end)/085dc458(nop;fall end)。
        // goto 落点保留独立块（4.4 直写 continue 会被 threading 折叠）。
        if (P_SEATS(i) == 0)
            goto SKIP_RANK_SEAT;
        if (P_TEAMS(i) == 0xff)
            goto SKIP_RANK_TEAM;
        if (!m_recvFlagB4.IsRecv(i))
        {
            m_fieldC0 = 0;
            register CUser* user = P_SEATS(i);
            G_CGameManager()->CheckOutPvp(user, true);
            changed = 1;
        }
SKIP_RANK_TEAM:
        ;
SKIP_RANK_SEAT:
        ;
    }
    return changed;
}
