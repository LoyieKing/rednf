// ORIG clear 0x08961376：置 +0/+4 为 0、+8/+9 为 0，再 clear 两个 string 与
// 两个 vector。
void HeroMissionCondition::clear()
{
    m_missionType = 0;
    m_field04 = 0;
    m_isRepeatMission = 0;
    m_isResetState = 0;
    m_mailTitle.clear();
    m_mailContent.clear();
    m_rewards.clear();
    m_extra.clear();
}