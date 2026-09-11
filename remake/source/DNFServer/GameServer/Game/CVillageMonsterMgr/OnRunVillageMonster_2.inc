// ============================================================================
// CVillageMonsterMgr::OnRunVillageMonster —— ORIG 0x86b44fc
//   未开战（m_field04==0）返回；各区域轮转；注册下一个 0x85 轮转定时器。
// ============================================================================
void CVillageMonsterMgr::OnRunVillageMonster()
{
    if (m_field04 == 0)
        return;
    for (std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.begin();
         it != m_areas.end(); ++it)
    {
        it->second.OnRunVillageMonster();
    }
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x85,
                                1, 0, 0);
}