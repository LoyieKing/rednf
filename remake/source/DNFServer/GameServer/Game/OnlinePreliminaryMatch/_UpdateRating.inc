void COnlinePreliminary::_UpdateRating(int winOrLose, ENUM_ELO_RATING_SYSTEM sys)
{
    // ORIG 0x08560be2：两 float 顶部声明 =0.0f（入口预清 -0xc/-0x10），
    // 双臂 _GetRating 序 (自,对) 互换 + CEloRating::calc(引用双参) +
    // _SetRating(队,自评)。原按值/selfTeam 三局部结构不收敛。
    float selfRating = 0.0f;
    float otherRating = 0.0f;
    if (winOrLose == 1)
    {
        selfRating = (float)_GetRating((ENUM_SCHOOL_MATCH_TEAM)0);
        otherRating = (float)_GetRating((ENUM_SCHOOL_MATCH_TEAM)1);
        CEloRating::calc(selfRating, otherRating, sys,
                         PvPGlobalEnvironmentVariable::school_match_elo_k_);
        _SetRating((ENUM_SCHOOL_MATCH_TEAM)0, selfRating);
    }
    else
    {
        selfRating = (float)_GetRating((ENUM_SCHOOL_MATCH_TEAM)1);
        otherRating = (float)_GetRating((ENUM_SCHOOL_MATCH_TEAM)0);
        CEloRating::calc(selfRating, otherRating, sys,
                         PvPGlobalEnvironmentVariable::school_match_elo_k_);
        _SetRating((ENUM_SCHOOL_MATCH_TEAM)1, selfRating);
    }
}