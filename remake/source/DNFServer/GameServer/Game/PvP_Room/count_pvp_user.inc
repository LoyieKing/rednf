// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::count_pvp_user（原 TU 第 792 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

int PvP_Room::count_pvp_user()
{
    // ORIG 0x085da840：continue 形产生 true 尾 jmp merge + false nop 岛。
    int local_c = 0;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) == 0)
        {
            continue;
        }
        local_c = local_c + 1;
    }
    return local_c;
}
