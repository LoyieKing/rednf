void CSkill::set_prelearned_skill()
{
    std::list<_pre_skill>::iterator it;
    int count = (int)m_preSkillList.size();
    CSkill* found;
    int i;
    _pre_skill* pSkill;
    it = m_preSkillList.begin();
    for (i = 0; i < count; ++i)
    {
        pSkill = &*it;
        found = G_CDataManager()->m_skillList->find_skill(
            m_jobType, pSkill->m_index);
        if (found != 0)
        {
            pSkill->m_group = found->get_group();
        }
        it++;
    }
}