// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_phase_total_cnt_of_small_round(short)
//   —— ORIG T 0x08306c0e：阶段总数 = STAssignBloodSystem.m_totalCnt（+0x24）。
// ---------------------------------------------------------------------------
int CBattle_Field::get_blood_spawn_phase_total_cnt_of_small_round(short smallRound)
{
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08306c0e：同 get_blood_spawn_time_of_phase
    // （result 先声明占 -0x10、sysIdx int 占 -0xc，signed 比较 mem 直比）。
    if (m_bloodPhaseTimes.empty() || m_bloodSystems.empty())
        return -1;
    // [b28d-39] ORIG 0x08306c0e：短整比较形（同 get_blood_spawn_cnt_of_phase）。
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
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08306c0e：`cmp -0xc(%ebp),%eax;
    // setle` 正写 + signed 内存直比。
    if ((int)m_bloodSystems.size() <= sysIdx)
        return -1;
    result = m_bloodSystems[sysIdx].m_totalCnt;
    return result;
}