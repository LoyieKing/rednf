int CParty::switchPostion(char a, char b)
{
    cMember tmp = m_member[a];
    m_member[a] = m_member[b];
    m_member[b] = tmp;
    return 1;
