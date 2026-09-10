stTitleRewardBasis* CAchievement::_getPostalReward(stTitleSection* section)
{
    unsigned int unused = 0;
    unsigned int i = 0;   // ORIG i 槽 -0x10（死变量 -0x14 先声明占位）
    while (i < section->m_rewards.size())
    {
        stTitleRewardBasis* reward = section->m_rewards[i];
        if ((reward != 0) && (reward->m_type == 3))
        {
            return reward;
        }
        ++i;
    }
    return 0;
