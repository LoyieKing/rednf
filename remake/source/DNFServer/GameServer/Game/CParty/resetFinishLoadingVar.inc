void CParty::resetFinishLoadingVar()
{
    for (int i = 0; i <= 3; i++)
    {
        m_battleData.m_arr50[i] = 0;
    }
}