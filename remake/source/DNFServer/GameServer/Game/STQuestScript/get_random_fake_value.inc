int TrainingQuestScript::get_random_fake_value(int idx, int day)
{
    return m_selectProb.m_dayProb[day].at(idx);
}