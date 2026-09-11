// ---------------------------------------------------------------------------
// CParty::GetSetPlayResultCount —— ORIG T 0x085b231e（报告
//   GetSetPlayResultCount.md）：已回传结算成员数。
// ---------------------------------------------------------------------------
int CParty::GetSetPlayResultCount()
{
    int count = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (m_member[i].m_pUser != 0 && m_member[i].m_field4 != 0)
            ++count;
    }
    return count;
}