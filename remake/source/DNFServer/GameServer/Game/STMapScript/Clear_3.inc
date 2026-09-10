void STAssignDefensePhaseTime::Clear()      // ORIG 0x089e85a8
{
    m_smallRoundOrder = 0;
    m_phaseIdx = -1;
    m_interval = 0;
    m_time = 1.0f;
    m_field10 = 0;
    m_spawnCnt = 0;                         // 无 +0x18 字节写（布局止于 +0x14）
