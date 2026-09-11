void CGameManager::SendWarRoomList(CUser* user)
{
    PacketGuard guard;
    guard.put_header(0, 0x57);
    int index = guard.get_index();
    guard.put_short(0);
    int count = 0;
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.begin();
    // b28d-solo-CGameManager 判词：AE（while 形翻正——S154「N82附 槽互换」判词作废）。
    // 探针 /tmp/solo_probe5.cpp：4.4.6 对 while/for 形产「入口 jmp 底部条件」+ it++
    // 临时浅槽/end 深槽（与 ORIG 同：it++ -0x18 < end -0x20），goto 尾标签形则相反
    // （it++ -0x20 > end -0x18）且多一条 jmp 后 nop。
    while (it != m_warRoomMap.end())
    {
        WarRoom* room = it->second;
        if (room != 0)
        {
            room->MakeRoomInfo((char*)&guard);
            count = count + 1;
        }
        it++;
    }
    guard.put_short(index, count);
    guard.finalize(true);
    user->Send(guard);
}