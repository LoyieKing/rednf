void CUser::ClearMissionKind(short kind)
{
    CMissionList_Charac* ml = (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    ml->ClearMissionKind(*this, kind);
