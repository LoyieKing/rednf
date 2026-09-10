WarRoom* CGameManager::FindJoinableWarRoom(CUser* user)
{
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.begin();
    // b28d-solo-CGameManager 判词：AE（while 形翻正——S154「N82附 槽互换」判词作废；
    // 探针 /tmp/solo_probe5.cpp h_while：入口 jmp 底部条件 + it++ 临时浅/end 深）。
    while (it != m_warRoomMap.end())
    {
        WarRoom* room = it->second;
        // P225 推广：ORIG 0x082a3316 两段独立守卫（cmpl/je + call/test/sete/test/je），
        // `&&` 合并会被 4.4 折成 mov$1/mov$0 物化 ⇒ 拆嵌套 if。
        if (room != 0)
        {
            if (room->IsJoinable(user) == 0)
                return room;
        }
        it++;
    }
    return 0;
