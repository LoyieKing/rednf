int CBattle_Field::get_blood_spawn_cnt_of_phase(short smallRound, short phase)
{
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08306a62：
    // ①声明序 cnt(-0x18)/startCnt(-0x14)/sysIdx(-0x10)/p(-0xc)（4.4 同声明序
    //   先声明深，P212 全头部声明）；
    // ②sysIdx 为 int ⇒ signed mem 直比（P209）；
    // ③startCnt 循环外取一次、m_cntInc 每轮重调 operator[]（P157 逐字段重读）。
    if (m_bloodPhaseTimes.empty() || m_bloodSystems.empty())
        return -1;
    // [b28d-39] ORIG 0x08306a62：`(short)size()` 与 short 形参 16 位比较
    // （cmp -0x1c(%ebp),%ax），避免 4.4 为 size_t 提升把 movswl 值缓存进 %ebx。
    if (smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    int cnt = 0;
    int startCnt = 0;
    int sysIdx;
    int p;
    sysIdx = (int)m_bloodPhaseTimes[smallRound].m_phaseIdx;
    if (sysIdx == -1)
    {
        if (m_bloodPhaseOrder.empty() || smallRound >= (short)m_bloodPhaseOrder.size())
            return -1;
        sysIdx = (int)m_bloodPhaseOrder[smallRound];
    }
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08306a62：`cmp -0x10(%ebp),%eax;
    // setle` 正写 + signed 内存直比。
    if ((int)m_bloodSystems.size() <= sysIdx)
        return -1;
    startCnt = m_bloodSystems[sysIdx].m_startCnt;
    for (p = 0; p < (int)phase + 1; ++p)
        cnt = startCnt + m_bloodSystems[sysIdx].m_cntInc * p;
    return cnt;
