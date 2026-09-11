// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_spawn_small_round_interval(short)
//   —— ORIG T 0x083066ea：小轮间隔 = phaseTime.m_interval（+0x08）。
// ---------------------------------------------------------------------------
int CBattle_Field::get_blood_spawn_small_round_interval(short smallRound)
{
    // [b28d-39] ORIG 0x083066ea：empty() 走独立早返（`je` 后直落 size 检查），
    // 仅 size/负值两条件走 `||` 短路物化（与另三个 getter 的合并短路形不同）。
    if (m_bloodPhaseTimes.empty())
        return -1;
    if (smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    int result = m_bloodPhaseTimes[smallRound].m_interval;
    return result;
}