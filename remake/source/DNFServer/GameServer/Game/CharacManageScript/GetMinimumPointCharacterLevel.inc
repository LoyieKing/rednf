int CharacManageScript::GetMinimumPointCharacterLevel()
{
    if (m_pointBonus.empty())
        return 0x28;
    return m_pointBonus[0].m_field00;
