void CDailyScheduleManager::AddDailSchedule(const std::vector<_DailySchedule>& schedule)
{
    // ORIG 0x08125c3c：逐条 push_back 到 m_schedules(+0xd8)。
    const unsigned int count = schedule.size();
    for (unsigned int i = 0; i < count; ++i)
        m_schedules.push_back(schedule[i]);
}