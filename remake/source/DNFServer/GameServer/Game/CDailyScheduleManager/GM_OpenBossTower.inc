// ORIG 0x8126cb8 / 0x8126d0c：BossTower 打开/关闭（两组：0xdb1/type0 + 0x505a/type1）。
void CDailyScheduleManager::GM_OpenBossTower()
{
    GM_OpenSchedule(0xdb1, (_DailyScheduleType)0, m_bossTowerA);
    GM_OpenSchedule(0x505a, (_DailyScheduleType)1, m_bossTowerB);
}