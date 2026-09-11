// ORIG W 0x086976f6：this+8+(jobIdx*0x46+levelIdx+0x44)*4 → m_rewardPrimary 下标。
int CLevelRewardTable::GetRewardItemNo(int jobIdx, int levelIdx)
{
    // [b28d-PoolSmall 归零] 旧 P231「纯 register 形参」旋钮是**反效果**（P248 裸
    // register 反例）：带 register 时 4.4 把 jobIdx 装 %edx、levelIdx 装 %eax，
    // 随后 this 又要装 %eax ⇒ 多出 `mov %edx,%ecx; mov %eax,%edx` 两条 regmove
    // （13 条 DIFF）。**删 register** 后 4.4 自然分配 job→%ecx / lv→%edx /
    // this→%eax，逐条复现 ORIG 0x086976f6 的
    // `mov 0xc(%ebp),%ecx; mov 0x10(%ebp),%edx; mov 0x8(%ebp),%eax;
    //  imul $0x46,%ecx,%ecx; lea (%ecx,%edx,1),%edx; add $0x44,%edx;
    //  mov 0x8(%eax,%edx,4),%eax` ⇒ 11=11 IDENTICAL。
    return m_rewardPrimary[jobIdx][levelIdx];   // [b28d-Main·检查点17] 二维下标（头已改 [9][0x46]）
}