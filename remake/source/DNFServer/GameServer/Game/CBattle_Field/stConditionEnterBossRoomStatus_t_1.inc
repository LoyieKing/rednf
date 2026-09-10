CBattle_Field::stConditionEnterBossRoomStatus_t::stConditionEnterBossRoomStatus_t(
    const stConditionEnterBossRoomStatus_t& other)
    : m_monsterIdx(other.m_monsterIdx),
      m_pos(other.m_pos),
      m_curKillCount(other.m_curKillCount),
      m_maxKillCount(other.m_maxKillCount)
{
