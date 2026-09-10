int CItemMakingSkill::GetPrevExpGap(int level)
{
    const stItemMakingSkill* table =
        reinterpret_cast<const stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
    unsigned int idx = static_cast<unsigned int>(level - 1);
    if (table->m_vec18.size() <= idx || idx == 0)
        return 0;
    return table->m_vec18[idx].first - table->m_vec18[idx - 1].first;
