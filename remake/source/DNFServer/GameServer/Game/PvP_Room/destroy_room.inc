// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::destroy_room（原 TU 第 1554 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

CUser* PvP_Room::destroy_room(int idx)
{
    // ORIG 0x085d81d0：get_index() 在 CSwitchLog 构造前求值并以 callee-saved
    // 跨构造调用缓存（ORIG +0x07..+0x12），日志首参为 __PRETTY_FUNCTION__
    // 局部静态符号（_ZZ…E19__PRETTY_FUNCTION__，P118/P139）。
    register int roomIdx = get_index();
    CUser* local_14;
    {
        CSwitchLog local_24(__PRETTY_FUNCTION__, 0x721, 0, 0);
        local_24("pvp@log room(%d), %d", roomIdx, idx);
    }
    local_14 = P_SEATS(idx);
    if (local_14 == 0)
    {
        for (int local_10 = 0; local_10 <= 7; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                local_14 = P_SEATS(local_10);
                break;
            }
        }
    }
    reset();
    return local_14;
}
