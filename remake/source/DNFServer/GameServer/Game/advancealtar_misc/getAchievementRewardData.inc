RewardParameter::getAchievementRewardData(int type) const
{
    // b28c-L5：ORIG 0x0889b544 块排布 = `operator== 命中(==end) → je 落「命中分支」`，
    //   即原码为**否定早返** `if (it == end) return 0;`（return 0 岛在 ptEv 之前，
    //   尾 `jmp` 汇合）。4.4 对 `if (it != end) {...} return 0;` 产镜像排布
    //   （ptEv 在前、mov $0 在尾）→ 6 行块序差。P222 双向：本例负极性早返即正解。
    std::map<int, AchievementRewardData>::const_iterator it = m_rewardMap.find(type);
    if (it == m_rewardMap.end())
    {
        return 0;
    }
    return const_cast<AchievementRewardData*>(&it->second);
}