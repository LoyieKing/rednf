bool CMission::_isAcceptCondition_Series(const CUser& user) const
{
    // [b28d-70] 判词：AE 56/57（pair 化后逐条同形）。
    CMissionList_Charac* list =
        (CMissionList_Charac*)user.GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (list->isClearMission(GetMissionIndex())) return 0;
    if (GetPrevMissionIndex() == -1) return 1;
    if (!list->isClearMission(GetPrevMissionIndex())) return 0;
    if (list->getIndex_byKind(GetMissionKind()) != 0) return 0;
    return 1;
}