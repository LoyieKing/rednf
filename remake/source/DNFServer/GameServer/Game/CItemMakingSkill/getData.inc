void CItemMakingSkill::getData(char* pData) const
{
    ItemMakingSkillSaveView* out = reinterpret_cast<ItemMakingSkillSaveView*>(pData);
    for (int i = 0; i <= 0xa; ++i)
        out->m_skillLevels[i] = m_skillLevels[i];
}