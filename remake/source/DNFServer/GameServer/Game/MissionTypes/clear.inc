// ORIG 0x85e68b6 W：4×movw 清零（kind/index/count/streak）
void MissionInfo::clear()
{
    m_kind = 0;
    m_index = 0;
    m_count = 0;
    m_streak = 0;
}