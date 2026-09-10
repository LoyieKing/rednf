// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::GetTeamIndex（原 TU 第 706 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

int PvP_Room::GetTeamIndex(CUser* user)
{
    // ORIG 0x085dc6ee：for+jmp-check+break 岛；-0x4=游标、-0x8=结果。
    int local_8;
    int local_4;
    for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1)
    {
        if (P_SEATS(local_4) == 0)
        {
            continue;
        }
        if (P_SEATS(local_4) == user)
        {
            break;
        }
    }
    if (local_4 == 8)
    {
        return -1;
    }
    if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
    {
        local_8 = local_4;
    }
    else
    {
        local_8 = P_TEAMWIN(local_4);
    }
    return local_8;
}
