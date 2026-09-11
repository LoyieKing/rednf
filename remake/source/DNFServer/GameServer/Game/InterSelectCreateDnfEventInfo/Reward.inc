// ---- Reward ----
// ORIG 0x8160c4a：+0x00 short（rate），+0x02 char（flag）。
InterSelectCreateDnfEventInfo::Reward::Reward(int rate, bool flag)
{
    m_rate = (unsigned short)rate;
    m_flag = flag;
}