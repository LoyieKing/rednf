// _GetRating ORIG W 0x08568966 —— m_teams[team].GetTeamRating() 取 float
float CFairMatch::_GetRating(ENUM_SCHOOL_MATCH_TEAM team)
{
    return static_cast<float>(m_teams[team].GetTeamRating());
}