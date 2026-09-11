// ============================================================================
// 公会战房间
// ============================================================================
WarRoom* CGameManager::GetWarRoom()
{
    WarRoom* room = m_warRoomPool.Acquire();
    if (room == 0)
        return 0;
    room->SetIndex(m_warRoomPool.GetIndex(room));
    int roomIndex = room->GetIndex();
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.find(roomIndex);
    if (it == m_warRoomMap.end())
    {
        m_warRoomMap[room->GetIndex()] = room;
        return room;
    }
    else
    {
        return 0;
    }
}