int CParty::get_total_level()
{
    int total = 0;
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i))
        {
            total += m_member[i].m_pUser->get_charac_level();
        }
    }
    return total;
