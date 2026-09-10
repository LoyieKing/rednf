float CFairMatch::GetGap(IMatch* other)
{
    return __builtin_fabsf(
        _GetRating(static_cast<ENUM_SCHOOL_MATCH_TEAM>(0)) -
        static_cast<CFairMatch*>(other)->_GetRating(
            static_cast<ENUM_SCHOOL_MATCH_TEAM>(0)));
