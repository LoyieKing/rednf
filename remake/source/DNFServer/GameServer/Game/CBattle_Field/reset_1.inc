void CBattle_Field::CBloodClearRewardData::reset()
{
    m_bloodRound = 0;
    m_playTime = 0;
    m_prevPlayTime = 0;
    m_isBloodClear = 0;
    for (int i = 0; i <= 3; ++i)
    {
        m_rewardExp[i] = 0;
        m_reward[i].reset();
    }
    m_ultimateClearPoint = 0;
}