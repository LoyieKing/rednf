bool CMission_remains_hp::_isAcceptCondition(const CUser& user) const
{
    CMissionList_Charac* list =
        (CMissionList_Charac*)user.GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (list->isClearMission(GetMissionIndex())) return 0;
    if (user.get_charac_job() == m_job) return true;
    return false;
