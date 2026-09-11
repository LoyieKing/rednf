// OnReward ORIG T 0x85665966?（README 0x08565966）报告 OnReward.md
void CFairMatch::OnReward(PvP_Room* room, int winTeam, bool draw)
{
    if (draw) {
        _UpdateRating(1, static_cast<ENUM_ELO_RATING_SYSTEM>(1));
        _UpdateRating(2, static_cast<ENUM_ELO_RATING_SYSTEM>(1));
        UpdateScore(room, m_teams[1].m_players[0], static_cast<ENUM_SCHOOL_MATCH_TEAM>(0),
                    0, 0, 1);
        UpdateScore(room, m_teams[0].m_players[0], static_cast<ENUM_SCHOOL_MATCH_TEAM>(1),
                    0, 0, 1);
    } else if (winTeam == 1) {
        _UpdateRating(1, static_cast<ENUM_ELO_RATING_SYSTEM>(2));
        _UpdateRating(2, static_cast<ENUM_ELO_RATING_SYSTEM>(0));
        UpdateScore(room, m_teams[1].m_players[0], static_cast<ENUM_SCHOOL_MATCH_TEAM>(0),
                    1, 0, 0);
        UpdateScore(room, m_teams[0].m_players[0], static_cast<ENUM_SCHOOL_MATCH_TEAM>(1),
                    0, 1, 0);
    } else {
        _UpdateRating(2, static_cast<ENUM_ELO_RATING_SYSTEM>(2));
        _UpdateRating(1, static_cast<ENUM_ELO_RATING_SYSTEM>(0));
        UpdateScore(room, m_teams[1].m_players[0], static_cast<ENUM_SCHOOL_MATCH_TEAM>(0),
                    0, 1, 0);
        UpdateScore(room, m_teams[0].m_players[0], static_cast<ENUM_SCHOOL_MATCH_TEAM>(1),
                    1, 0, 0);
    }
    ++m_fieldB0;
    PacketGuard pkt;
    pkt.put_header(0, 0x13c);
    pkt.put_int(3 - m_fieldB0);
    pkt.finalize(true);
    room->send_to_pvp(pkt);
}