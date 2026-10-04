void CItemMakingSkill::getData(char* pData) const
{
    SIG_ITEM_MAKING_SKILL* out = reinterpret_cast<SIG_ITEM_MAKING_SKILL*>(pData);
    for (int i = 0; i <= 0xa; ++i)
        out->m_skillLevels[i] = m_skillLevels[i];
}