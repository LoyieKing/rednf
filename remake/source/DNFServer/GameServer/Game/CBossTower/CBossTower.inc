// [b28f-SweepCBossTower P327] ctor/dtor 定义迁自 CDeathTower.cpp（ORIG 归属证据：
//   ORIG `_GLOBAL__I_` 键名——本 TU = WongWork::CBossTower::CBossTower()@0x0814557d，
//   CDeathTower.cpp TU = CDeathTower::CPacketHandler::makePickupItemHeader@0x0846991d；
//   ORIG ctor 0x081429b4 / dtor 0x08142a74 位于本 TU 区段，紧邻 _onTimerProcess）。
// 具名字段访问依据（ORIG 报告 WongWork__CBossTower/CBossTower.md @0x81429b4 /
// ~CBossTower.md @0x8142a74）：
//   +0xaf0 = CBossTower::m_idx（movl 清 0）
//   +8     = CBossPlay::m_field8（movl 清 0）
//   +0xd   = CBossPlay::m_memberAlive[4]（ORIG memset(0xd,0,4) → 保持 memset 形态）
//   +0x9f0 = CBossStage::m_dungeon（ORIG movl $0x0,0x9f0(%eax) 单条 dword 直存）
//   +4     = CBossPlay::m_party（movl 清 0）
//   +0xb00 = CBossTower::m_dungeonLog（CBossDungeonEntranceLog，0x18，编译器构造/析构）
// 继承：CBossTower : CBossStage : CBossPlay（隐式基类构造/析构，编译器处理 vptr）。
CBossTower::CBossTower()
{
    CBossStage::reset();
    m_idx = 0;
    m_field8 = 0;
    memset(m_memberAlive, 0, 4);
    m_dungeon = 0;   // ORIG 0x081429b4 证据 movl $0x0,0x9f0(%eax)：单条 dword 直存，非 memset call
    m_party = 0;
    // m_dungeonLog（CBossDungeonEntranceLog）由编译器在进入函数体前按权威
    // ctor（ORIG 0x081456c2：map C1 + reset）构造，此处不再 memset。
}