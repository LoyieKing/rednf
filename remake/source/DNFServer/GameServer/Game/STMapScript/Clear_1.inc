void STAssignBloodPhaseTime::Clear()        // ORIG 0x089e84d0
{
    m_smallRoundOrder = 0;                  // +0x00
    m_phaseIdx = -1;                        // +0x04（-1 = 随机）
    m_interval = 0;                         // +0x08
    m_time = 1.0f;                          // +0x0c mov $0x3f800000
    m_field10 = 0;
    m_spawnCnt = 0;
    m_difficulty = 0;                       // +0x18 movb $0
