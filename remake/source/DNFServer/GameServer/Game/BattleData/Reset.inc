void BattleData::Reset()
{
    ((BattleDataZeroView*)this)->a[0] = 0;
    ((BattleDataZeroView*)this)->a[1] = 0;
    ((BattleDataZeroView*)this)->a[2] = 0;
    ((BattleDataZeroView*)this)->a[3] = 0;
    ((BattleDataZeroView*)this)->a[4] = 0;
    ((BattleDataZeroView*)this)->b[0] = 0;   // 0x2c/0x30 成对
    ((BattleDataZeroView*)this)->b[1] = 0;   // 0x34/0x38 成对
    m_killedMonster0 = 0;                    // ORIG 0x3c..0x4c 各独立单 store
    m_killedMonster1 = 0;
    m_killedMonster2 = 0;
    m_field48 = 0;
    m_field4c = 0;
    m_flag28[0] = 0;
    m_flag28[1] = 0;
    m_flag28[2] = 0;
    m_flag28[3] = 0;
    m_field68c = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_arr50[i] = 0;
        m_arr54[i] = 0;
        m_arr58[i] = 0;
        m_arr5c[i] = 0;
        m_arr60[i] = 0;
        m_gameResults[i].Clear();
        m_arr7e8[i] = 0x7fffffff;
        m_arr64[i] = 0;
        reinterpret_cast<Inven_Item*>(&m_itemGroups0[i][0])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups0[i][1])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups1[i][0])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups1[i][1])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups2[i][0])->reset();
        reinterpret_cast<Inven_Item*>(&m_itemGroups2[i][1])->reset();
        m_arr7d0[i] = 0;
        m_arr7e4[i] = 0;
    }
