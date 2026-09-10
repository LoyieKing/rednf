void STAssignBloodSystem::Clear()           // ORIG 0x089e8454
{
    m_id = -1;                              // +0x00 movl $-1
    m_field04 = 0;
    m_phase = 0;
    m_field0c = 0;
    m_field10 = 0;
    m_timeA = 0;
    m_timeB = 0;
    m_startCnt = 0;
    m_cntInc = 0;
    m_totalCnt = 0;                         // +0x24
