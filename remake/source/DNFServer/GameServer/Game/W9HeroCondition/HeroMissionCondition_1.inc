// ORIG copy ctor 0x08964f5c：拷贝 +0..+0x9 标量后逐成员拷贝构造两 string、
// 两 vector（vector<RewardData> copy ctor = ORIG C1ERKS3_）。
HeroMissionCondition::HeroMissionCondition(const HeroMissionCondition& other)
    : m_missionType(other.m_missionType),
      m_field04(other.m_field04),
      m_isRepeatMission(other.m_isRepeatMission),
      m_isResetState(other.m_isResetState),
      m_mailTitle(other.m_mailTitle),
      m_mailContent(other.m_mailContent),
      m_rewards(other.m_rewards),
      m_extra(other.m_extra)
{
}