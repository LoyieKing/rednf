void CDailyScheduleManager::GM_CloseBossTower()
{
    GM_CloseSchedule(0xdb1, (_DailyScheduleType)0, m_bossTowerA);
    GM_CloseSchedule(0x505a, (_DailyScheduleType)1, m_bossTowerB);
