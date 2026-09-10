void TrainingQuestScript::clear()
{
    m_selectProb.m_count = 0;
    m_selectProb.m_pairs.clear();
    m_applyCount = 0;
    for (int i = 0; i <= 0x45; ++i)
    {
        m_questProb[i].clear();
    }
    m_sameApplyQuest.clear();
