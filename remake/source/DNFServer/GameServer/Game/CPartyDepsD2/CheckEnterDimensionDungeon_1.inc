int CParty::CheckEnterDimensionDungeon(CDungeon const* dungeon, unsigned char& flag,
                                     char mode)
{
    bool allowed = true;                                 // movb $1,-0xd

    // ORIG 以 3 参构造 DimensionPartyCount((char)m_battleField.m_bloodState, mode, -1)
    // （ctor W 0x085beabe），非默认构造+赋值
    DimensionPartyCount count((char)m_battleField.m_bloodState, mode, (char)-1);

    if (dungeon->get_dimension_member_count(count))      // T 0x0834d220
    {   // (signed char)count.m_2 <= memberCount → 不可入场
        const int members = get_member_count();
        if (!((int)count.m_2 > members))
            allowed = false;
    }
    return CheckEnterDimensionDungeon(dungeon, flag, allowed);   // 尾调 bool 重载
