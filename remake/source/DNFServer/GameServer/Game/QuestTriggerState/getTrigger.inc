int stQuestTriggerState_t::getTrigger()
{
    int r = m_trigger0;
    r |= (m_trigger1 << 9);
    r |= (m_trigger2 << 18);
    return r;
