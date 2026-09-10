int CParty::CheckMemberArea()
{
    int area = -1;
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (area == -1)
        {
            area = m_member[i].m_pUser->get_area(false);
        }
        else if (area != m_member[i].m_pUser->get_area(false))
        {
            return 0;
        }
    }
    return 1;
