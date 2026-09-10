// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::check_joinable（原 TU 第 633 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

int PvP_Room::check_joinable(CUser* user)
{
    // [b28d-7] P16 早返结构：ORIG 0x085dbd3c `je BODY; mov$4; jmp END; BODY:`
    // ⇒ 失败块（return 4）物理居前，if/else 形反产 jne+双 return 4 尾。
    if (m_state != 1)
    {
        return 4;
    }
    int local_10 = 0;
    if ((local_10 = ((IMatch*)m_matching)->CheckJoinable(this, user)) != 0)
    {
        return local_10;
    }
    for (int local_f4 = 0; local_f4 < 8; local_f4 = local_f4 + 1)
    {
        if ((P_SEATS(local_f4) == 0) && (m_teams[local_f4] == 0xff))
        {
            return 0;
        }
    }
    return 4;
}
