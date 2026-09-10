bool Arad_VillageAttackRewardItem::find_reward_item(int winPoint, Data& out)
{
    // ORIG 0x81a3e52：std::find<iterator,Data>(sret -0x18, begin -0x10, end -0x14, out)。
    (void)winPoint;   // ORIG 未使用该形参（比较值取自 out）
    std::vector<Data>::iterator it =
        std::find(m_rewardList.begin(), m_rewardList.end(), out);
    if (it != m_rewardList.end())
    {
        out = *it;
        return true;
    }
    return false;
