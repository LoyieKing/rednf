int CParty::get_live_count()
{
    int count = 0;
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i) != 1)
            continue;
        if (m_battleData.m_arr54[i] != 0)
        {
            count++;
        }
    }
    return count;
}