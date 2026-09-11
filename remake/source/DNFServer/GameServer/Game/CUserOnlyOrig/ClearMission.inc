// ---------------------------------------------------------------------------
// 47) CUser::ClearMission(short missionId)
//     报告：docs/class_func_reports/CUser/ClearMission.md。ORIG 0x08662606。
// ---------------------------------------------------------------------------
void CUser::ClearMission(short missionId)
{
    // ORIG 0x08662606：`call isGMUser; xor $0x1,%eax; test %al,%al; jne <ret岛>` ⇒ !bool 早退形
    if (!isGMUser())
        return;
    {
        CMissionList_Charac* ml = (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
        ml->ClearMission(*this, missionId);
    }
}