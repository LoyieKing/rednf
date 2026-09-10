PvP_Room* CGameManager::GetPvp()
{
    // b28d-lane12: CSwitchLog 改临时链（P221）无净降（62）；残余 Guard/CSwitchLog/
    // find sret 槽组平移 → N82附。ORIG 0x0829507c。
    // b28d-solo-CGameManager 判词：N82附（帧 0x64+push ebx vs 0x68；room -0xc vs -0x10）。
    PvP_Room* room = m_pvpRoomPool.Acquire();
    if (room == 0)
        return 0;
    room->SetIDX(m_pvpRoomPool.GetIndex(room));
    int roomIndex = room->get_index();
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.find(roomIndex);
    if (it == m_pvpRoomMap.end())
    {
        // b28d-laneCGM（P87/P227）：ORIG 0x08295134 `call get_index; mov %eax,%ebx` 在
        //   CSwitchLog ctor **前**预取（op() 实参用 %ebx）；具名 roomIdx 恒落槽。
        CSwitchLog(__PRETTY_FUNCTION__, 0xaa9, 0, 0)(
            "pvp@log room(%d) new alloc", room->get_index());
        m_pvpRoomMap[room->get_index()] = room;
        return room;
    }
    else
    {
        CSwitchLog(__PRETTY_FUNCTION__, 0xab2, 0, 0)(
            "pvp@log room(%d) already alloc", room->get_index());
        return 0;
    }
