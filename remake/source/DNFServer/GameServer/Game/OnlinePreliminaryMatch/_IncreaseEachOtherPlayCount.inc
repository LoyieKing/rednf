void COnlinePreliminary::_IncreaseEachOtherPlayCount(CUser* a, CUser* b)
{
    if ((char)GetPlayCountPerOneTeam() == 1)
    {
        /* ORIG @085602ca gcn(b)→%ebx、@085602d7 gsg(b)→%esi 跨 expand call 保全
         * ——P52 单语句全内联实参形复现（具名 no/grp 反退化帧，b28 N96 系误判）。 */
        reinterpret_cast<COnlinePreliminaryMemberMatchList*>(
            a->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)5))
            ->IncreasePeerPlayCount(b->GetServerGroup(), b->getCurCharacNo());
    }
    else if (a->GetCurCharacOnlinePreliminaryTeam() != 0)
    {
        a->GetCurCharacOnlinePreliminaryTeam()->IncreaseEachOtherTeamPlayCount(
            b->GetCurCharacOnlinePreliminaryTeamId());
    }
}