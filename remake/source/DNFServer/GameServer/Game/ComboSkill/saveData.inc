bool ComboSkill::saveData(SIG_COMBO_SKILL* data) const
{
    if (data == 0)
    {
        return 0;
    }
    convertComboSkillArray(data->m_tree1Count, data->m_tree1Data,
                           SKILL_TREE_1);
    convertComboSkillArray(data->m_tree2Count, data->m_tree2Data,
                           SKILL_TREE_2);
    return 1;
