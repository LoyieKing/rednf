unsigned int COnlinePreliminary::_GetEachOtherPlayCount(CUser* a, CUser* b)
{
    if ((char)GetPlayCountPerOneTeam() == 1)
    {
        // 在 a 的“对过谁”表（charac_expand(5)）中记/查 b 的 (serverGroup,characNo)
        /* ORIG @0856022e gcn(b)→%ebx、@0856023b gsg(b)→%esi 跨 expand call 保全
         * （push esi;push ebx;sub $0x10）——P52 单语句全内联实参形复现（具名 no/grp
         * 反落栈槽退化 sub $0x24，本件 b28 N96 系误判，P0 内联复测翻正）。 */
        return reinterpret_cast<COnlinePreliminaryMemberMatchList*>(
                   a->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)5))
                   ->GetPeerPlayCount(b->GetServerGroup(), b->getCurCharacNo());
    }
    /* ORIG @08560272 test;setne %al;test;je→mov $0 岛(@+0x8f)：!=0 正向 */
    if (a->GetCurCharacOnlinePreliminaryTeam() != 0)
        return a->GetCurCharacOnlinePreliminaryTeam()->GetEachOtherTeamPlayCount(
            b->GetCurCharacOnlinePreliminaryTeamId());
    return 0;
}