int CBattle_Field::get_blood_spawn_cnt_of_small_round(short smallRound)
{
    // [b28d-39] ORIG 0x08306670：同 get_blood_round_of_small_round 的 `||` 短路形。
    if (m_bloodPhaseTimes.empty() ||
        smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    int result = m_bloodPhaseTimes[smallRound].m_spawnCnt;
    return result;
