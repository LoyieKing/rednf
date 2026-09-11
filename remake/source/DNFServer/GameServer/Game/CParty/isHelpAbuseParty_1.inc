// ORIG T 0x085a21d4（0x71 字节）：任一有效成员 isHelpAbuseParty 则真。
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
}