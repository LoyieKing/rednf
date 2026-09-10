bool CParty::isHelpAbuseParty()
{
    for (int i = 0; i <= 3; ++i)
    {
        if (!checkValidUser(i))
        {
            continue;
        }
        CUser* user = get_user(i);
        if (isHelpAbuseParty(user))
        {
            return true;
        }
    }
    return false;
