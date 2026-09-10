bool CParty::getDungeonDropTestFlag()
{
    for (int i = 0; i < 4; ++i)
    {
        CUser* user = get_user(i);                       // thunk 0x08145764
        if (user == 0)
            continue;
        if (user->isGMUser() && getDungeonMapSaving())
            return true;
    }
    return false;
