void CParty::OnFailedDungeonClear(CUser* user)
{
    OnDungeonClear(user, false);                         // movl $0x0,0x8(%esp)
