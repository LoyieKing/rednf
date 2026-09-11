MultiBoxLotteryInfo::LotteryUnit::LotteryUnit(const LotteryUnit& other)
    : m_count(other.m_count), m_defaultIdx(other.m_defaultIdx),
      m_defaultNum(other.m_defaultNum), m_defaultFlag(other.m_defaultFlag),
      m_items(other.m_items), m_bonus(other.m_bonus)
{
}