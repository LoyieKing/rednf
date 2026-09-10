void STSpecialMonsterDrop::clear()
{
    // ORIG 0x8513a24：m_enable 字节直写 0 + memset(+0x01,0,5) + memset(+0x06,0,3)。
    m_enable = 0;
    memset(m_difficulty, 0, sizeof(m_difficulty));
    memset(m_dungeonType, 0, sizeof(m_dungeonType));
