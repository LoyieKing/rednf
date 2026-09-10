void CCoinRefillEvent::StartEvent(Word_Param param)
{
    // [b28d-59 判词] 封 N82（ORIG 0x0832be82）：122/132 条、P167 调用点 6/6 CLEAR
    // （0 缺 0 多）⇒ 语义完成；残差=callee-saved 组（ORIG push edi/esi/ebx）+
    // 物化次数。byte 层封顶。
    SetEventFlag(true);
    memset(&m_timeCount, -1, 0x14);
    m_times[0] = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c 当前小时槽
    m_times[1] = param.m_duration;               // 低 ushort
    m_times[2] = param.m_field2;                 // 高 ushort
    m_timeCount = 0;
    while ((unsigned int)m_timeCount <= 3u)
    {
        if (m_times[m_timeCount] > 0x17)
        {
            m_times[m_timeCount] = -1;
        }
        if (m_times[m_timeCount] == -1)
        {
            break;
        }
        m_timeCount = m_timeCount + 1;
    }
    for (int i = 1; i <= m_timeCount; i = i + 1)
    {
        int minVal = 0x19;
        int minIdx = 5;
        for (int j = i; j <= m_timeCount; j = j + 1)
        {
            if (m_times[j] < minVal)
            {
                minVal = m_times[j];
                minIdx = j;
            }
        }
        m_times[minIdx] = m_times[i];
        m_times[i] = minVal;
    }
    registNextRefillTimer();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x48, 0);
    trace("Coin Refill Event Start! time(%d) time(%d) time(%d)",
          m_times[0], m_times[1], m_times[2]);
