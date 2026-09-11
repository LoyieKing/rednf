void CCoinRefillEvent::StartEvent(Word_Param param)
{
    // [b28d-59 判词] （ORIG 0x0832be82）：122/132 条、P167 调用点 6/6 CLEAR。
    // [L6S2b 修 · 视角 + 语义] ORIG 全部槽访问走 m_slots 单数组视角（基址 +0xc）：
    //   ①while 段 `mov 0xc(%eax),%eax; lea 0x1(%eax),%edx; mov 0xc(%eax,%edx,4)`
    //     = `m_slots[m_timeCount + 1]`（旧译 `m_times[m_timeCount]` 走 +0x10 基址，
    //     地址虽同但少一条 lea）；
    //   ②for 段 `mov 0xc(%eax,%edx,4)`（0x0832bf78/bf87，edx=j=j..m_timeCount，j 从
    //     i=1 起）= `m_slots[j]`；旧译 `m_times[j]` 基址 +0x10 ⇒ **下标整体错 1 且
    //     j=m_timeCount=4 时读到 +0x20（union 只到 +0x1f）= 越界读**（运行期真 bug）。
    //   交换段同理 `m_slots[minIdx]`/`m_slots[i]`（0x0832bfb1/bfc5）。
    // [L6S2b 修 · P227] ORIG 三个 trace 实参在 cMyTrace ctor **之前**右起求值并驻
    //   %edi/%esi/%ebx 保活（`mov 0x18(%eax),%edi; mov 0x14(%eax),%esi;
    //   mov 0x10(%eax),%ebx` @0x0832bfef..bffb，prologue push edi/esi/ebx）⇒ 具名
    //   两语句形坍缩为 ctor 后 ecx/edx/eax 装载；改全表达式临时链即复现。
    SetEventFlag(true);
    memset(&m_timeCount, -1, 0x14);
    m_times[0] = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c 当前小时槽
    m_times[1] = param.m_duration;               // 低 ushort
    m_times[2] = param.m_field2;                 // 高 ushort
    m_timeCount = 0;
    while ((unsigned int)m_timeCount <= 3u)
    {
        if (m_slots[m_timeCount + 1] > 0x17)
        {
            m_slots[m_timeCount + 1] = -1;
        }
        if (m_slots[m_timeCount + 1] == -1)
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
            if (m_slots[j] < minVal)
            {
                minVal = m_slots[j];
                minIdx = j;
            }
        }
        m_slots[minIdx] = m_slots[i];
        m_slots[i] = minVal;
    }
    registNextRefillTimer();
    cMyTrace(__PRETTY_FUNCTION__, 0x48, 0)(
        "Coin Refill Event Start! time(%d) time(%d) time(%d)",
        m_times[0], m_times[1], m_times[2]);
}