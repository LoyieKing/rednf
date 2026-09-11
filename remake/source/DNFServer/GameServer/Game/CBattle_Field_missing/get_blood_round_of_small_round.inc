// ---------------------------------------------------------------------------
// CBattle_Field::get_blood_round_of_small_round(short)
//   —— ORIG T 0x083065f8：小轮对应大轮 = phaseTime.m_smallRoundOrder。
// ---------------------------------------------------------------------------
int CBattle_Field::get_blood_round_of_small_round(short smallRound)
{
    // [b28d-39] ORIG 0x083065f8：`||` 短路链（empty / `(short)size()<=smallRound` /
    // `smallRound<0`）汇到 invalid 岛，`mov $1/%eax`+`mov $0` 物化后 test；具名 int
    // result（尾 mov %eax,-0xc; mov -0xc,%eax）。
    if (m_bloodPhaseTimes.empty() ||
        smallRound >= (short)m_bloodPhaseTimes.size() || smallRound < 0)
        return -1;
    int result = m_bloodPhaseTimes[smallRound].m_smallRoundOrder;
    return result;
}