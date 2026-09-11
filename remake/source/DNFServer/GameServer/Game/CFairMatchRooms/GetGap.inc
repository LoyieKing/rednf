// GetGap ORIG T 0x856491c 报告 GetGap.md —— 双 call 内联差：4.4 实测
// `__builtin_fabsf(f() - g())` 直出 `call; fstps -0xc; call; fsubrs -0xc; fabs`。
float CFairMatch::GetGap(IMatch* other)
{
    return __builtin_fabsf(
        _GetRating(static_cast<ENUM_SCHOOL_MATCH_TEAM>(0)) -
        static_cast<CFairMatch*>(other)->_GetRating(
            static_cast<ENUM_SCHOOL_MATCH_TEAM>(0)));
}