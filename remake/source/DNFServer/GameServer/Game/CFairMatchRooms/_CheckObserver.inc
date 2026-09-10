bool CFairMatch::_CheckObserver(PvP_Room* room, CUser* /*user*/)
{
    int observerCnt = 0;
    for (int seat = 0; seat <= 7; ++seat) {
        if (room->get_team(seat) == 3)   // 队伍号 3 = 观战席
            ++observerCnt;
    }
    // ORIG 尾部：size() 返回值 ebx 缓存，call 后 movzbl %al 收窄 playCount，
    // cmp/setge 两分支：size-count >= playCount → 0，否则 → 1
    if (static_cast<int>(m_teams[0].size()) - observerCnt
        >= static_cast<int>(static_cast<unsigned char>(GetPlayCountPerOneTeam())))
        return 0;
    return 1;
