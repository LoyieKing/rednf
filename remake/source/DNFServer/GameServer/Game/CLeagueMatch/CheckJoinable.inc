int CLeagueMatch::CheckJoinable(PvP_Room* room, CUser* user)   // ORIG T 0x0855cf8c
{
    if (room->get_player_count() >= (int)(nPlayerCountPerOneTeam_ * 2))
        return 4;
    if (m_teamA.size() + m_teamB.size() > (int)(nPlayerCountPerOneTeam_ * 2))
        return 4;

    switch (m_state) {
    case 0: {
        // [b28c-21 代落 C28L25 spec] ORIG `call _CheckSchool; xor $1,%eax;
        // test %al,%al; je` = bool callee 裸形（P168a①）；int callee 恒 sete。
        if (!_CheckSchool(room, user))
            return 4;
        return 0;
    }
    case 3:
        return 0;
    case 1:
    case 4:
    case 5:
        return 4;
    default:
        return 4;
    }
}