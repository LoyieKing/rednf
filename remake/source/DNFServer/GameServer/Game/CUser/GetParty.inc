CParty* CUser::GetParty()
{
    // ORIG 0x8655160 js 尾岛（0x8655182 mov $0）：正向 early-return（probe12 B 形）
    if (m_partyIndex >= 0)
        return G_CGameManager()->GetParty(m_partyIndex);
    return 0;
}