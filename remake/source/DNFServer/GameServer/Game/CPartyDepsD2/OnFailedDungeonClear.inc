// ---------------------------------------------------------------------------
// OnFailedDungeonClear —— ORIG W 0x085bf7da
// 无独立报告（objdump 转写，已读同类调用者 giveup_game/onBloodClear/
// TournamentDungeonClear/dungeon_clear/ClearDungeon 报告确认消费形态）：
// 3 条指令 thunk → OnDungeonClear(user, false)。
// ---------------------------------------------------------------------------
void CParty::OnFailedDungeonClear(CUser* user)
{
    OnDungeonClear(user, false);                         // movl $0x0,0x8(%esp)
}