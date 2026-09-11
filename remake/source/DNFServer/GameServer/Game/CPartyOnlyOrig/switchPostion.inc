// ORIG T 0x085ba680（switchPostion.md）：整个 cMember 槽（0x18 字节含
// m_pUser）对调。
int CParty::switchPostion(char a, char b)
{
    cMember tmp = m_member[a];
    m_member[a] = m_member[b];
    m_member[b] = tmp;
    return 1;
}