bool CMission_within::_isAcceptCondition(const CUser& user) const
{
    // [b28d-70] 判词：AE 86=86（GetRankRange().first 三处 sret 形不变）。
    CMissionList_Charac* list =
        (CMissionList_Charac*)user.GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (list->isClearMission(GetMissionIndex())) return 0;
    // b28d-50：ORIG 0x085e3c30 三处均**调 GetRankRange() 取 .gradeMin**（0x085e692a
    // sret 落 -0x2c/-0x24/-0x1c 三个临时槽），不是直读 m_gradeMin；且尾比较为
    // `curPoint + (nextPoint - curPoint)` 两步冗余式（0x085e3d1a sub/add 段，P171
    // 照抄）。按 ORIG 改后 86/63 gap 闭合。
    if (user.getPVPResultRefR()->m_pvpGrade != GetRankRange().first) return 0;

    int curPoint = GlobalData::s_ref_pvp_grade->GetPvpPoint(GetRankRange().first);
    int nextPoint =
        GlobalData::s_ref_pvp_grade->GetPvpNextRankPoint(GetRankRange().first);
    if (user.getPVPResultRefR()->m_expPoint >= curPoint + (nextPoint - curPoint))
    {
        const_cast<CUser&>(user).getPVPResultRefW()->m_expPoint = nextPoint;
        return 1;
    }
    return 0;
