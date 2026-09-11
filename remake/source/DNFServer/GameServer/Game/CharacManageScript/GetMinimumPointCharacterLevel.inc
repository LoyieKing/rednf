// ---------------------------------------------------------------------------
// CharacManageScript::GetMinimumPointCharacterLevel —— ORIG T 0x088e1266
// m_pointBonus 空 → 0x28；否则取首行 m_field00。
// ---------------------------------------------------------------------------
int CharacManageScript::GetMinimumPointCharacterLevel()
{
    if (m_pointBonus.empty())
        return 0x28;
    return m_pointBonus[0].m_field00;
}