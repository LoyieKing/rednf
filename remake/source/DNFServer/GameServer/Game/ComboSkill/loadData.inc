bool ComboSkill::loadData(SIG_COMBO_SKILL* data)
{
    if (data == 0)
    {
        return 0;
    }
    if (data->m_tree1Count != 0)
    {
        convertComboSkillMap(data->m_tree1Count, data->m_tree1Data,
                             SKILL_TREE_1);
    }
    if (data->m_tree2Count != 0)
    {
        convertComboSkillMap(data->m_tree2Count, data->m_tree2Data,
                             SKILL_TREE_2);
    }
    return 1;
}