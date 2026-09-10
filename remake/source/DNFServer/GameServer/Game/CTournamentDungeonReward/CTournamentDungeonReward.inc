CTournamentDungeonReward::CTournamentDungeonReward()
    : m_rewards()
{
    // ORIG 0x8284770：构造 2x2 m_rewards 后调 ResetReward()（各 Clear + m_field04=0）
    ResetReward();
