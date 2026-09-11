bool Quest::IsOpenScheduleQuest() const
{
    int state = 0;
    int result = GlobalData::s_DailyScheduleManager->GetScheduleState(
        (_DailyScheduleType)1, (unsigned int)m_index);
    return (result == 1) || (result == 2);
}