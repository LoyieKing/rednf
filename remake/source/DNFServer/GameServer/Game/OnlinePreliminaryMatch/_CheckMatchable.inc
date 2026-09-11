int COnlinePreliminary::_CheckMatchable(PvP_Room* room) const
{
    int observers = 0;
    int firstTeamId = 0;
    for (int i = 0; i <= 7; ++i)
    {
        if (room->get_team(i) == 3)
        {
            ++observers;
            continue;
        }
        CUser* u = reinterpret_cast<CUser*>(room->get_user_seat(i));
        if (u == 0)
            continue;
        /* ORIG @08561327 firstTeamId==0 分支内独立 call(@+0x6d) 赋 -0x14 后
         * jmp 增量块(@+0x75)；else @+0x7d 再次 call + cmp -0x14,%eax; setne
         * 物化(@+0x85【技巧22】)；log 分支 @+0x92 第三次 call */
        if (firstTeamId == 0)
        {
            firstTeamId = u->GetCurCharacOnlinePreliminaryTeamId();
            continue;
        }
        /* [b28d-solo-OnlinePreliminaryMatch] ORIG @08561348 `cmp -0x14(%ebp),%eax;
         * setne %al; test %al,%al; je`（物化后直测、无槽）；具名 bool 局部在
         * 4.4 恒落 -0x9 槽（mov%al,-N;cmpb），故写裸比较表达式（P168a② 直测形）。
         * ORIG test %al,%al; je→增量块(@+0xcf)，log 体走 fall-through(@+0x8c)：
         * 正向 if 不取反。 */
        if (u->GetCurCharacOnlinePreliminaryTeamId() != firstTeamId)
        {
        /* ORIG @08561352 第三次 call 结果直落 0x14(%esp)(@+0x97)，无具名
         * 中转折算槽【技巧36/37】 */
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x771, "(%d) found a different team_id(%d)",
            u->GetCurCharacOnlinePreliminaryTeamId());
        return 0x16;
        }
    }
    /* ORIG 尾 @085613c5 setne 物化 + 双常量岛(@0x16/jmp/mov$0)；
     * byte 化用 (unsigned char) 强转产 movzbl(@085613c0) */
    /* [b28d-solo-OnlinePreliminaryMatch] ORIG @085613c0 `movzbl %al,%eax;
     * cmp %eax,%ebx; setne %al; test %al,%al; je`（尾物化无槽）⇒ 裸比较。 */
    if (m_teams[0].size() - observers != (int)(unsigned char)GetPlayCountPerOneTeam())
        return 0x16;
    return 0;
}