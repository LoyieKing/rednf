// ---------------------------------------------------------------------------
// CBattle_Field::CBloodClearRewardData::setRewarItemCopy(Inven_Item, int, int)
//   —— ORIG T 0x085bf176：m_reward[rewardIdx].m_item[itemIdx].setCopy(item)。
// ---------------------------------------------------------------------------
void CBattle_Field::CBloodClearRewardData::setRewarItemCopy(Inven_Item item,
                                                            int rewardIdx,
                                                            int itemIdx)
{
    m_reward[rewardIdx].m_item[itemIdx].setCopy(item);
}