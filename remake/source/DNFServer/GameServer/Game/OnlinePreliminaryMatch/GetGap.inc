float COnlinePreliminary::GetGap(IMatch* other)
{
    /* ORIG @08561a14 float 局部 0.0f 两段物化到 -0x8【技巧43】；具名指针
     * 中转 other→-0x4(@08561a1c)【技巧13】；首 call 结果 fstps -0x14、次
     * call 结果免 spill 直接 fsubrs(@08561a4b) */
    float theirs = 0.0f;   // ORIG -0x8 死局部（写 0 后未被读）
    COnlinePreliminary* o = static_cast<COnlinePreliminary*>(other);
    return fabsf(_GetRating(static_cast<ENUM_SCHOOL_MATCH_TEAM>(0))
                 - o->_GetRating(static_cast<ENUM_SCHOOL_MATCH_TEAM>(0)));
}