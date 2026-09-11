// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_time_of_next_small_round(short)
//   —— ORIG T 0x0830676a：下一小轮时间 = m_timeA - m_totalCnt * m_timeB。
// ---------------------------------------------------------------------------
int CBattle_Field::get_blood_spawn_time_of_next_small_round(short smallRound)
{
    // [b28d-solo-CBattle_Field_missing] ORIG 0x0830676a：同 get_blood_spawn_time_of_phase
    // （result 先声明占 -0x10、sysIdx int 占 -0xc，signed 比较 mem 直比）。
    if (m_bloodPhaseTimes.empty() || m_bloodSystems.empty())
        return -1;
    // [b28d-39] ORIG 0x0830676a：短整比较形（同 get_blood_spawn_cnt_of_phase）。
    if (smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    // result(-0x10, 预清 0) 先声明、sysIdx(-0xc) 次声明（ORIG 槽序）。
    int result = 0;
    int sysIdx;
    sysIdx = (int)m_bloodPhaseTimes[smallRound].m_phaseIdx;
    if (sysIdx == -1)
    {
        if (m_bloodPhaseOrder.empty() || smallRound >= (short)m_bloodPhaseOrder.size())
            return -1;
        sysIdx = (int)m_bloodPhaseOrder[smallRound];
    }
    // [b28d-solo-CBattle_Field_missing] ORIG 0x0830676a：`cmp -0xc(%ebp),%eax;
    // setle` 正写 + signed 内存直比。
    if ((int)m_bloodSystems.size() <= sysIdx)
        return -1;
    // [b28d-solo-CBattle_Field_missing] P157/P183：ORIG 三次独立 operator[]
    // （+0x14 timeA → +0x18 timeB → +0x24 totalCnt），timeA/timeB 各跨后续调用
    // 保活 %ebx/%esi ⇒ 逐字段表达式重读，禁引用缓存；乘法书写序 timeB*totalCnt
    // 复现 `imul %esi,%eax`（totalCnt 落 %eax、timeB 落 %esi）。
    result = m_bloodSystems[sysIdx].m_timeA
           - m_bloodSystems[sysIdx].m_timeB * m_bloodSystems[sysIdx].m_totalCnt;
    return result;
}