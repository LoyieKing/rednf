int CLevelRewardTable::GetItemCount(int jobIdx, int levelIdx)
{
    // [b28d-PoolSmall 归零] 同 GetRewardItemNo：删 register 形参即逐条复现
    // ORIG 0x08697712（`imul $0x46,%ecx,%ecx; lea (%ecx,%edx,1),%edx;
    //  add $0x2bc,%edx; mov (%eax,%edx,4),%eax`，m_rewardSecondary@+0xaf0 无
    // disp）⇒ 11=11 IDENTICAL。带 register 时同样多 2 条 regmove。
    return m_rewardSecondary[jobIdx][levelIdx];   // [b28d-Main·检查点17] 同上
