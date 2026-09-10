void WarAreaCounter::clear()
{
    memset(m_counterTable, 0, sizeof(m_counterTable));
    memset(m_timeHourList, 0, sizeof(m_timeHourList));
    memset(m_doorFee, 0, sizeof(m_doorFee));
    m_curHourIdx = 0;
    m_banInterval = 0;
    m_kickPoint = 0;
    m_readyCount = 0;
    memset(m_warAreaDungeon, 0, sizeof(m_warAreaDungeon));
    m_wpPerMonsterHp.clear();
    m_num = 0;
    // ORIG 尾段：+0x140=num 后 memset(+0x144,0,0x3c0)、清 +0x138、+0x13c。
    memset(m_warRoomCount, 0, sizeof(m_warRoomCount));
    m_wpPenalty = 0;
    m_monsterExpConst = 0;
