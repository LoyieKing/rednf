stLevelUpRewardItemScript::GetGrowUpRewardItemFunctor::GetGrowUpRewardItemFunctor(
    const LevelUpRewardItemTableKey& key, std::vector<LevelUpRewardItemData>& out)
    : m_key(&key), m_out(&out)
{
