bool ARAD::SCRIPT::AradJumping_Script::getCurRewardItem(int job, int growValue,
                                                        unsigned long& out)
{
    std::vector<ARAD::SCRIPT::_AradJumpingRewardItem>::iterator it =
        m_rewardItems.begin();
    std::vector<ARAD::SCRIPT::_AradJumpingRewardItem>::iterator end =
        m_rewardItems.end();
    while (it != end)
    {
        if (it->m_job == job && it->m_growValue == growValue)
        {
            out = it->m_money;
            break;
        }
        ++it;
    }
    if (it != end)
        return true;
    return false;
