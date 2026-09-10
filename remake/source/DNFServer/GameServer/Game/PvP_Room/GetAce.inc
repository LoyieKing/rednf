// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::GetAce（原 TU 第 4064 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

int PvP_Room::GetAce() const
{
    // ORIG 0x085dc654：m_field98[seat] 为主判据、m_field78[seat] 为平局决胜
    // （mov 0x8(%eax,(seat+0x24)*4) = this+seat*4+0x98；+0x1c → +0x78），
    // 房间内嵌数组，非 CUser 对象字段。
    int local_10 = -1;
    int local_c = -1;
    for (int local_8 = 0; local_8 < 8; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) != 0)
        {
            if (local_c < m_field98[local_8])
            {
                local_c = m_field98[local_8];
                local_10 = local_8;
            }
            else if ((m_field98[local_8] == local_c) &&
                     (m_field78[local_10] < m_field78[local_8]))
            {
                local_10 = local_8;
            }
        }
    }
    return local_10;
}
