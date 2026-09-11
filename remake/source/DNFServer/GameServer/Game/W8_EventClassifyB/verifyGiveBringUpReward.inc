// verifyGiveBringUpReward（ORIG 0x0810fe1a）
// ORIG 分支形态：multiple==0 分支 if/return（P56）；否则 OR 短路链直接 jcc
// 汇聚统一 fail 出口（P133），Ghidra else-if 链。
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
}