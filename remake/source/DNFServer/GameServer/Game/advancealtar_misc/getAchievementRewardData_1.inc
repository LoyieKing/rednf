RewardParameter::getAchievementRewardData(StageDifficulty::T diff, int count) const
{
    for (std::map<int, AchievementRewardData>::const_iterator it = m_rewardMap.begin();
         it != m_rewardMap.end(); it++)
    {
        // b28d-35：ORIG 0x0889b5a8 每字段重新计算 `it->second` 基址
        //   （`call operator->` 后直接 `mov 0x8(%eax)` / 再调一次后 `mov 0xc(%eax)`）
        //   ⇒ P183 禁引用缓存：删除 `const AchievementRewardData& data` 具名缓存。
        // b28d-solo-advancealtar_misc：ORIG 0x0889b5d0 首比较为
        //   `mov 0x8(%eax),%eax; mov %eax,%edx; mov 0xc(%ebp),%eax; cmp %eax,%edx`
        //   —— 两侧显式 `(unsigned int)` cast 使 4.4 保留 eax→edx 中转 + 形参入
        //   %eax 的寄存器比较（探针 /tmp/enp4.cpp k1；`(int)` cast 或裸比较恒折
        //   单段 `mov 0x8(%eax),%edx` + 内存比较）。第二比较（int vs int）保持
        //   裸形，与 ORIG `mov 0xc(%eax),%eax; cmp 0x10(%ebp),%eax` 一致。
        if ((unsigned int)it->second.m_difficulty == (unsigned int)diff &&
            it->second.m_stageClearCount == count)
        {
            return const_cast<AchievementRewardData*>(&it->second);
        }
    }
    return 0;
}