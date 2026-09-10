void CUser::ResetDailyMission()
{
    CMissionList_Charac* ml = (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)(8));
    ml->Reset_DailyMission();
