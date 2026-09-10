ENUM_SCHOOL_MATCH_TEAM CFairMatch::_GetOtherTeam(ENUM_SCHOOL_MATCH_TEAM team)
{
    if (team == (ENUM_SCHOOL_MATCH_TEAM)0)
        return (ENUM_SCHOOL_MATCH_TEAM)1;
    if (team == (ENUM_SCHOOL_MATCH_TEAM)1)
        return (ENUM_SCHOOL_MATCH_TEAM)0;
    return (ENUM_SCHOOL_MATCH_TEAM)2;
