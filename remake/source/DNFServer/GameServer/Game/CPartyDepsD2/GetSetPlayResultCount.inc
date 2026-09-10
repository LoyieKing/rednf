int CParty::GetSetPlayResultCount()
{
    int count = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (m_member[i].m_pUser != 0 && m_member[i].m_field4 != 0)
            ++count;
    }
    return count;
