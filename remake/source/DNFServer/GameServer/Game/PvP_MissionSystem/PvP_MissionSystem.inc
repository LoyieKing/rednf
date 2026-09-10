PvP_MissionSystem::PvP_MissionSystem()
    : m_missionMap(),
      m_kindMissionList(),
      m_dailyMissionIndices(),
      m_dailyMissionList(),
      m_dailyMissionKind(),
      m_baseMissionExp()
{
    // b28d-61 [ORIG 0x085e1b16-0x085e1b5d `movl $0x0,(%esp); call time; mov %eax,-0x24;
    // lea -0x24,%esi; movl $0x9c8,(%esp); call new; call CMTRand::C1ERKm`]：直接
    // `new CMTRand(time(0))`（ctor 吃 unsigned long const&，临时占槽 -0x24），
    // 非「默认构造 + seed()」两段。
    m_trand = new CMTRand((unsigned long)time(0));
    m_field3c = 0;
    m_dailyMissionKind.reset();
    // ORIG 0x085e1b78-0x085e1b9b：先构造空 list 临时（-0x20），再调
    // vector::resize(0x24, list)（085e7444 按值传第二参）——非单参 resize。
    m_kindMissionList.resize(0x24, std::list<CMission*>());
