void CParty::OnSuccessedDungeonClear(CUser* user)
{
    OnDungeonClear(user, true);                          // movl $0x1,0x8(%esp)
