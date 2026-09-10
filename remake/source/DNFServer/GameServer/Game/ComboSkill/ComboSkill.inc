ComboSkill::ComboSkill()
{
    std::vector<std::pair<int, int> >* pComboSkillList =
        (std::vector<std::pair<int, int> >*)(
            ((CDataManagerComboLayout*)G_CDataManager())->m_pComboList + 0x48fc);
    std::vector<int> vecSkill;
    vecSkill.resize(6);
    for (int i = 0; i < 6; ++i)
    {
        m_comboSkillMap.insert(
            std::make_pair((*pComboSkillList)[i].first, vecSkill));
        m_comboSkillSfpMap.insert(
            std::make_pair((*pComboSkillList)[i].first, vecSkill));
    }
