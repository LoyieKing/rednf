int UserQuest::set_trigger(int questIdx, char type, bool bIncrement)
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            if (m_user->getGmQuestFlag())
            {
                m_trigger[i] = 0;
            }

            if (type == 1)
            {
                ++m_trigger[i];   // b25c：++ 形 4.4 单次装载 i（探针 probe_inc s3），ORIG 同构
            }
            else
            {
                if (!check_quest_condition(questIdx))
                {
                    return -1;
                }
                if (bIncrement)
                {
                    stQuestTriggerState_t state;
                    state = m_trigger[i];
                    state.incTirggerState(type);
                    m_trigger[i] = state.getTrigger();
                }
                else if (m_trigger[i] > 0)
                {
                    stQuestTriggerState_t state;
                    state = m_trigger[i];
                    state.subTriggerState(type);
                    m_trigger[i] = state.getTrigger();
                }
            }
            return m_trigger[i];
        }
    }
    return -1;
}