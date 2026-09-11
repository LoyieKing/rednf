// ============================================================================
// 构造（ORIG 0x85e1abc）
// 顺序：map@+0x00、vector<list<CMission*>>@+0x18、vector<uint>@+0x24、
//       list<pair<uint,uint>>@+0x34、bitset@+0x40、vector<uint>@+0x48；
//       m_trand = new CMTRand(time(0))；m_field3c=0；reset bitset；
//       m_kindMissionList.resize(0x24)。
// ============================================================================
// ORIG 0x85e1abc：+0x30(m_field30)/+0x3c(m_field3c)/+0x44(m_trand) 仅
// 构造函数体内赋值一次，初始化列表不产生写入（m_field30 保持垃圾，与 ORIG 一致）。
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
}