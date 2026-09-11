// ============================================================================
// GetPartyMemberForceDropFlag —— ORIG T 0x085bdc00。
// 任一成员 get_user(i)!=0 且 isGMUser 且 getForceDropFlag → 1。
// ============================================================================
int CParty::GetPartyMemberForceDropFlag()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = get_user(i);
        if (user == 0)
            continue;
        if (user->isGMUser() && user->getForceDropFlag())
            return 1;
    }
    return 0;
}