int CItemMakingSkill::GetSkillLevel(ENUM_ITEM_MAKING_TYPE type, CUser* user) const
{
    const stItemMakingSkill* table =
        reinterpret_cast<stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
    int idxByExp = 0;
    int idxByLevel = 0;
    for (idxByExp = 0;
         static_cast<int>(idxByExp) < static_cast<int>(table->m_vec18.size());)
    {
        if (static_cast<int>(static_cast<short>(m_skillLevels[type]))
            >= static_cast<int>(table->m_vec18[idxByExp].first))
        {
            ++idxByExp;
        }
        else
        {
            break;
        }
    }
    for (idxByLevel = 0;
         static_cast<int>(idxByLevel) < static_cast<int>(table->m_vec18.size());)
    {
        if (user->get_charac_level()
            >= static_cast<int>(table->m_vec18[idxByLevel].second))
        {
            ++idxByLevel;
        }
        else
        {
            break;
        }
    }
    return (idxByExp <= idxByLevel) ? idxByExp : idxByLevel;
