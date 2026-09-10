void CBattle_Field::CBloodClearRewardData::setRewardItemAddInfo(float rate,
                                                                int rewardIdx,
                                                                int itemIdx)
{
    // P183：ORIG 0x85bf1ae/0x85bf1f3 两段完整 imul 链重算地址，无引用缓存。
    // ORIG fildl/fistpl 复用同槽 -0xc：无中间局部嵌套形。
    m_reward[rewardIdx].m_item[itemIdx].set_add_info(
        (int)((float)m_reward[rewardIdx].m_item[itemIdx].get_add_info() * rate));
