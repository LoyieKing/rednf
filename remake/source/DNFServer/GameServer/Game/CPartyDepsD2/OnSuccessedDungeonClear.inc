// ---------------------------------------------------------------------------
// OnSuccessedDungeonClear —— ORIG W 0x085bf7fc
// 无独立报告（objdump 转写，依据同上）：thunk → OnDungeonClear(user, true)。
// ---------------------------------------------------------------------------
void CParty::OnSuccessedDungeonClear(CUser* user)
{
    OnDungeonClear(user, true);                          // movl $0x1,0x8(%esp)
}