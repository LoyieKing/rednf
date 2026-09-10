// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：IsDeathMatchMap（原 TU 第 71 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================
bool IsDeathMatchMap(int mapNo, bool& onlyDeathMatch)
{
    PvPMapInfo info;
    if (CMapList::GetPvpMapInfo(mapNo, info))
    {
        onlyDeathMatch = info.m_playable;
        if (mapNo > 0x13 || mapNo < 0)
            return 0;
        return PVPMAP_DEATHMATCH_MODE[mapNo];
    }
    return 0;
}
