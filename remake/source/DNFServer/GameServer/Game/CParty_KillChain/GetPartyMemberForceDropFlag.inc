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
