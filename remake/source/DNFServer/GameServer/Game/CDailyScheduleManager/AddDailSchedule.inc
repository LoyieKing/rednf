void CDailyScheduleManager::AddDailSchedule(const std::vector<_DailySchedule>& schedule)
{
    // ORIG 0x08125c3c：逐条 push_back 到 m_schedules(+0xd8)。
    const unsigned int count = schedule.size();
    for (unsigned int i = 0; i < count; ++i)
    {
        _DailySchedule e = schedule[i];
#if !remake_verify
        // Client HUD 领主之塔：16:00-18:00 / 21:00-24:00（etc 原文 00:00-23:59）。
        if (e.m_type == 4 || e.m_type == 0)
        {
            e.m_startHour = 16; e.m_startMin = 0; e.m_startSec2 = 0;
            e.m_endHour = 18; e.m_endMin = 0; e.m_endSec2 = 0;
        }
        else if (e.m_type == 1)
        {
            e.m_startHour = 21; e.m_startMin = 0; e.m_startSec2 = 0;
            e.m_endHour = 23; e.m_endMin = 59; e.m_endSec2 = 59;
        }
#endif
        m_schedules.push_back(e);
    }
}
