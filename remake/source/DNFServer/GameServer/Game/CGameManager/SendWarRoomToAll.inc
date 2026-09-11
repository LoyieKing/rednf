void CGameManager::SendWarRoomToAll(int index)
{
    PacketGuard guard;
    guard.put_header(0, 0x57);
    guard.put_short(1);
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.find(index);
    if (it == m_warRoomMap.end())
        return;
    WarRoom* room = it->second;
    room->MakeRoomInfo((char*)&guard);
    guard.finalize(true);
    G_GameWorld()->send_all(guard);
}