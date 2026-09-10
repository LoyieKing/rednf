void CDailyScheduleManager::GM_OpenBossTower()
{
    GM_OpenSchedule(0xdb1, (_DailyScheduleType)0, m_bossTowerA);
    GM_OpenSchedule(0x505a, (_DailyScheduleType)1, m_bossTowerB);
