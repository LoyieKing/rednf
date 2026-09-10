bool CUser::is_clear_stealingSkillMission() const
{
    // ORIG：GetCharacExpandData(8) 结果暂存 -0xc(%ebp) 后调
    // CMissionList_Charac::isClearStealingSkillMission
    //（mission_list_is_clear 为 stub 返回 false，禁用）
    void* data = GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    return reinterpret_cast<CMissionList_Charac*>(data)
        ->isClearStealingSkillMission();
