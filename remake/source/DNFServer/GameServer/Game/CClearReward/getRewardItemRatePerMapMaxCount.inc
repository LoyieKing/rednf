int CClearReward::getRewardItemRatePerMapMaxCount(int mapCount) const
{
    // P196：`if (mapCount <= 9) return arr[mapCount]; return 0;` 使数组访问 inline fall-through，
    // ORIG `cmpl $0x9; jg(返0岛)` + 内联 mov。
    if (mapCount <= 9)
        return m_rewardRatePerMap[mapCount];
    return 0;
}