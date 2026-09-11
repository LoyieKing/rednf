int CDeathTower::CPlayData::getRewardItemCount(int idx)
{
    // ORIG 0x8469bec：m_rewards[idx].m_itemCnt
    return m_rewards[idx].m_itemCnt;
}