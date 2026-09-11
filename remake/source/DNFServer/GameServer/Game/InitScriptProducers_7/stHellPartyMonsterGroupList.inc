// ORIG stHellPartyMonsterGroupList C1 0x891b1d6 W：memset(-1) 两段 + 三标量清零。
stHellPartyMonsterGroupList::stHellPartyMonsterGroupList()
{
    memset(m_monsterIdx, -1, sizeof(m_monsterIdx));
    memset(m_rate, -1, sizeof(m_rate));
    m_field50 = 0;
    m_field52 = 0;
    m_field54 = 0;
}