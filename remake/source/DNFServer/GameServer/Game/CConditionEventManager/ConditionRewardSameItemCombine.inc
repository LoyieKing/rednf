// ORIG T 0x08335264
// _ZN22CConditionEventManager30ConditionRewardSameItemCombineERSt6vectorI26stConditionEventRewardInfoSaIS1_EERSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
// 遍历 infos，按 info.m_field0（物品 id）聚合 m_field4（数量）到 outMap：
// 已存在则累加 second，否则插入 {m_field0, m_field4}。
void CConditionEventManager::ConditionRewardSameItemCombine(
    std::vector<stConditionEventRewardInfo>& infos, std::map<int, int>& outMap)
{
    std::vector<stConditionEventRewardInfo>::iterator it = infos.begin();
    for (; it != infos.end(); ++it)
    {
        std::map<int, int>::iterator mit = outMap.find(it->m_field0);
        if (mit == outMap.end())
        {
            outMap.insert(std::make_pair(it->m_field0, it->m_field4));
        }
        else
        {
            mit->second += it->m_field4;
        }
    }
}