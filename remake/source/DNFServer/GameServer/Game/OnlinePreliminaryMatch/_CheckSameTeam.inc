bool COnlinePreliminary::_CheckSameTeam(PvP_Room* /*room*/, CUser* user)
{
    /* ORIG @085606f2 cmp $-1,%eax; sete %al; test %al,%al; je —— 裸 if 直判即产
     * sete+test（无独立 bool 槽，帧 sub $0x14 与 ORIG 全等）；具名 bool 反 spill 到
     * -0xa/-0x9（本件 b28 前判 N96 系误判，P168a① 裸形复测翻正）。 */
    if (user->GetCurCharacOnlinePreliminaryTeamId() == -1)
        return 0;
    /* ORIG @0856070e 第二次 call 结果直落 %ebx、@0856071b 第三 call→%eax、
     * @0856071d cmp %eax,%ebx：与 _CheckObserver 尾同族——lhs call 跨 rhs
     * call 自动落 callee-saved；具名绑定/中转折算反产 mov 链 */
    if (user->GetCurCharacOnlinePreliminaryTeamId() != GetTeamId())
        return 0;
    return 1;
