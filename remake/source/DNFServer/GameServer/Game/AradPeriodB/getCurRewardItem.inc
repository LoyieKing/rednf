// ============================================================================
// ARAD::SCRIPT::AradJumping_Script::getCurRewardItem —— ORIG T 0x088b424a
//（报告 ARAD__SCRIPT__AradJumping_Script/getCurRewardItem.md 全文转写）：
// 线性扫 m_rewardItems；(m_job==job && m_growValue==growValue) → out=m_money、
// 返回 true；未命中返回 false。
// ORIG nm 为 `_ZN`（非 const 成员，无 `_ZNK` 版本）；头声明已同步去 const。
// ============================================================================
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
}