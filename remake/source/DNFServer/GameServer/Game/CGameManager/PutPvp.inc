void CGameManager::PutPvp(PvP_Room* room)
{
    // b28d-solo-CGameManager 判词：AE（P223 变体翻正——S154「N121 尾岛不可择」判词作废）。
    // ORIG 0x08295348 形状 = `je <nop 岛 0x101>; …; test %al; je <epilogue 0x102>;
    // …; jmp 0x102; 0x101: nop; 0x102:`——唯一复现形是**第一守卫裸早返**
    // `if (room == 0) return;` + 第二守卫正向包裹（探针 /tmp/solo_probe3.cpp g8
    // 在 4.4.6 下逐条同构；g2/g10 双早返会多出第二个 nop 岛、g1/g3 正向包裹无岛）。
    if (room == 0)
        return;
    // ORIG：roomIndex 经 ebx callee-saved 跨 CSwitchLog 构造缓存（P87）；
    // bare `register`（无 asm 绑定，P0 合规——仅槽+ebx 装载形）。
    register int roomIndex = room->get_index();
    CSwitchLog(__PRETTY_FUNCTION__, 0xae4, 0, 0)("pvp@log room(%d)", roomIndex);
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it =
            m_pvpRoomMap.find(room->get_index());
    if (it != m_pvpRoomMap.end())
    {
        m_pvpRoomMap.erase(room->get_index());
        m_pvpRoomPool.Free(room);
    }
}