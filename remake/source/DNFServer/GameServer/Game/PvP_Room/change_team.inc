int PvP_Room::change_team(int seat, int team)
{
    lock();
    P_TEAMS(seat) = team;
    m_relayMgr.OnChangeTeam(seat, team);
    unlock();
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}