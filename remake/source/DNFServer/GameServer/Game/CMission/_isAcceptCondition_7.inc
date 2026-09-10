bool CMission_move_channel_total::_isAcceptCondition(const CUser& user) const
{
    CMissionList_Charac* list =
        (CMissionList_Charac*)user.GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (list->getIndex_byKind(0x1b)) return 0;
    if (list->isClearMission(GetMissionIndex())) return 0;
    if (GetPrevMissionIndex() == -1) return 0;
    if (list->isClearMission(GetPrevMissionIndex())) return 0;
    if (list->getIndex_byKind(7)) return 0;
    return 1;
