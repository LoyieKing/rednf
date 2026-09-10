// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::calculate_pvp_point（原 TU 第 3105 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

int PvP_Room::calculate_pvp_point()
{
    m_userTable.Clear();
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        // ORIG 085d9090：call 后 test %al;jne（无 xor/归一）；if/else 两支尾
        // + observer 空 then 共三条 jmp merge（@41/@56/@57）。
        if (P_SEATS(local_10) == 0)
            goto CP_NEXT;
        if (IsPvpObserver(local_10))
        {
            goto CP_NEXT;
        }
        if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
        {
            m_userTable.Set(local_10, P_SEATS(local_10));
            goto CP_NEXT;
        }
        else
        {
            m_userTable.Set(P_TEAMWIN(local_10), P_SEATS(local_10));
            goto CP_NEXT;
        }
CP_NEXT:
        ;
    }
    m_userTable.Calculate();
}
