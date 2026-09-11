// ---------------------------------------------------------------------------
// HeroMissionCondition C1/C2 —— ORIG 0x081681c8：仅依次构造 +0x0c/+0x10
// std::string 与 +0x14/+0x20 vector；标量区不初始化。
HeroMissionCondition::HeroMissionCondition()
    : m_mailTitle(), m_mailContent(), m_rewards(), m_extra()
{
}