bool CObjectBringUp::verifyGiveBringUpReward(
    ObjectBringUpScript::ObjectBringUpRule& rule, int count)
{
    if (rule.m_rewardMutipleNumber == 0)
    {
        if (rule.m_rewardStartCount == count)
            return true;
        return false;
    }
    if (rule.m_rewardStartCount <= count)
    {
        if (rule.m_rewardEndCount >= count || rule.m_rewardEndCount == 0)
        {
            if ((count - rule.m_rewardStartCount) %
                    rule.m_rewardMutipleNumber == 0)
                return true;
        }
    }
    return false;
