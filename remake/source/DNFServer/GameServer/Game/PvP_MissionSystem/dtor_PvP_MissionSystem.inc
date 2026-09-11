// ============================================================================
// 析构（ORIG 0x85e1c48）
// 顺序：delete m_trand；m_trand=0；遍历 m_missionMap delete 各 CMission；
//       m_missionMap.clear(); 其余成员默认析构。
// ============================================================================
PvP_MissionSystem::~PvP_MissionSystem()
{
    if (m_trand != 0)
        delete (CMTRand*)m_trand;
    m_trand = 0;
    puts("Mission System Delete");   // ORIG 0x85e1c72-0x85e1c79

    for (std::map<int, CMission*>::iterator it = m_missionMap.begin();
         it != m_missionMap.end(); ++it)
    {
        // b28d-61 [P141/P157，ORIG 0x085e1c8x `mov 0x4(%eax),%eax; test %eax,%eax;
        // je`]：无具名局部、无显式判空——delete 自带空指针守卫；旧形多
        // `mov %eax,-0xc` + 双重 cmpl。
        delete it->second;
    }
    m_missionMap.clear();
}