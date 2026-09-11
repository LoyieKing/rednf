// _CheckRematchable(const) ORIG T 0x08563b16 报告 _CheckRematchable.md
bool CFairMatch::_CheckRematchable(PvP_Room* room) const
{
    int counts[3] = { 0, 0, 0 };   // [1]/[2] 计真实两队人数
    for (int seat = 0; seat <= 7; ++seat) {
        const int team = room->get_team(seat);
        if (team == 1 || team == 2)
            ++counts[team];
    }
    if (counts[1] != static_cast<int>(static_cast<unsigned char>(
            const_cast<CFairMatch*>(this)->GetPlayCountPerOneTeam())))
        return false;
    if (counts[2] != static_cast<int>(static_cast<unsigned char>(
            const_cast<CFairMatch*>(this)->GetPlayCountPerOneTeam())))
        return false;
    return true;
}