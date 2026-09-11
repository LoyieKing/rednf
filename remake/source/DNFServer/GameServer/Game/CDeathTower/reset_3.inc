void CDeathTower::CPlayData::reset()
{
    // ORIG 0x08461f18：m_playTime/m_lastPlayTime/m_lastRoutedTurn 清零；
    // 4 成员 alive/ready 清 0；m_rewards[4]（步长 0x268）逐个
    // stRewardItem_t::reset；m_rewardExp[4] 清 0；m_enterStartMap[4] 清 0。
    m_playTime = 0;
    m_lastPlayTime = 0;
    m_lastRoutedTurn = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_memberAlive[i] = 0;
        m_memberReady[i] = 0;
        m_rewards[i].reset();
        m_rewardExp[i] = 0;
    }
    for (int i = 0; i < 4; ++i)
        m_enterStartMap[i] = 0;
}