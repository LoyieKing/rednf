bool CLeagueMatch::_CheckSchool(PvP_Room* room, CUser* user)  // ORIG T 0x0855cef8
{
    if (user->GetSchoolNo() == 0)
        return 0;
    int emptySeats = 0;
    for (int i = 0; i <= 7; ++i) {
        if (room->get_team(i) == 3)
            ++emptySeats;
    }
    if ((int)(m_teamA.size() - emptySeats) >= (int)nPlayerCountPerOneTeam_)
        return 0;
    return 1;
