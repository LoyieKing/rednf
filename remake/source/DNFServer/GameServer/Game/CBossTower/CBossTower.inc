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
