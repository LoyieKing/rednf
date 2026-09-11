// ============================================================================
// CVillageMonsterMgr::OnCreateVillageMonster —— ORIG 0x86b4280
//   各区域生成怪物；m_field04=2；注册 0x85 轮转定时器（1 秒）。
// ============================================================================
void CVillageMonsterMgr::OnCreateVillageMonster()
{
    for (std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.begin();
         it != m_areas.end(); ++it)
    {
        it->second.OnCreateVillageMonster();
    }
    m_field04 = 2;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x85,
                                1, 0, 0);
}