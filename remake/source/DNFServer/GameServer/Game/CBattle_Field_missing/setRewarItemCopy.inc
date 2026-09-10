void CBattle_Field::CBloodClearRewardData::setRewarItemCopy(Inven_Item item,
                                                            int rewardIdx,
                                                            int itemIdx)
{
    m_reward[rewardIdx].m_item[itemIdx].setCopy(item);
