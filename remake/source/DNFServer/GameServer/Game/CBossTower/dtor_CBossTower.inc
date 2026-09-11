CBossTower::~CBossTower()
{
    CBossStage::reset();
    m_idx = 0;
    m_field8 = 0;
    memset(m_memberAlive, 0, 4);
    m_dungeon = 0;   // ORIG 0x08142a74 证据 movl $0x0,0x9f0(%eax)：单条 dword 直存
    m_party = 0;
    // m_dungeonLog 由编译器在函数体结束后按权威 dtor（ORIG 0x08145704：
    // reset + map D1）析构，此处不再 memset。
}