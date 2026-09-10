float CFairMatch::_GetRating(ENUM_SCHOOL_MATCH_TEAM team)
{
    return static_cast<float>(m_teams[team].GetTeamRating());
