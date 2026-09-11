void CGameManager::SendPvpList(CUser* user)
{
    PacketGuard guard;
    guard.put_header(0, 0x29);
    guard.put_short((int)m_pvpRoomMap.size());
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.begin();
    // b28d-solo-CGameManager 判词：AE（while 形翻正——S154「N82附 槽互换」判词作废；
    // 同 SendWarRoomList，探针 /tmp/solo_probe5.cpp h_while）。
    while (it != m_pvpRoomMap.end())
    {
        PvP_Room* room = it->second;
        if (room != 0)
        {
            if (room->get_waiter_count() > 0)
            {
                room->make_room_info((char*)&guard);
            }
        }
        it++;
    }
    guard.finalize(true);
    user->Send(guard);
}