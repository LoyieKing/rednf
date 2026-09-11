bool CUser::acceptable_within_mission() const
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    // ORIG 0x8??：grade 走 %ebx（`mov %eax,%ebx` 技巧 2），`mov %ebx,0x4(%esp)` 直传；
    // mission=-0x10 / idx=-0xc。b28d-solo-CUser：裸 register 复现 ebx 保活（P231）。
    register int grade = get_pvp_grade();
    int idx = G_CDataManager()->get_WithinMissionIndex(grade);
    if (idx == 0)
        return false;
    return mission->MakeMissionList_JustKind(*this, 0x1b);
}