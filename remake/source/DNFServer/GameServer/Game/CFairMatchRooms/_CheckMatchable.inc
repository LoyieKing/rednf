// _CheckMatchable(const) ORIG T 0x08563a54 报告 _CheckMatchable.md
int CFairMatch::_CheckMatchable(PvP_Room* room) const
{
    int observerCnt = 0;
    for (int seat = 0; seat <= 7; ++seat) {
        if (room->get_team(seat) == 3)
            ++observerCnt;
        CUser* u = reinterpret_cast<CUser*>(room->get_user_seat(seat));
        if (u != 0) {
            if (G_CDataManager()->GetMaxGradePvPChannel() <
                u->get_pvp_grade())
                return 0xd;
        }
    }
    // ORIG +0x99..0xb4：差值留 %ebx 跨 call（P52），(bool)(a > b) 复现 setg 物化
    // （t23/f3 实测；!=0 特例会被 4.4 折叠，> 不会）。
    if ((bool)(static_cast<int>(m_teams[0].size()) - observerCnt >
               static_cast<int>(static_cast<unsigned char>(
                   const_cast<CFairMatch*>(this)->GetPlayCountPerOneTeam()))))
    {
        return 0x16;
    }
    return 0;
}