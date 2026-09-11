void CGameManager::PutWarRoom(WarRoom* room)
{
    // b28d-lane12: 同 PutParty——ndiff 12→8；残余 eraseKey@-0x10 vs -0xc → N82附。
    // ORIG 0x082a2666。
    // b28d-solo-CGameManager: AE 达成——P0 内联右值形（ORIG 槽全为实参/sret 临时）。
    if (room == 0)
        return;
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it =
            m_warRoomMap.find(room->GetIndex());
    if (it != m_warRoomMap.end())
    {
        m_warRoomMap.erase(room->GetIndex());
        m_warRoomPool.Free(room);
    }
}