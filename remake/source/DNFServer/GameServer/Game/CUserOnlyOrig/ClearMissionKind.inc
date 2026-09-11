// ---------------------------------------------------------------------------
// 48) CUser::ClearMissionKind(short kind)
//     报告：docs/class_func_reports/CUser/ClearMissionKind.md。ORIG 0x086626a0。
// ---------------------------------------------------------------------------
void CUser::ClearMissionKind(short kind)
{
    CMissionList_Charac* ml = (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    ml->ClearMissionKind(*this, kind);
}