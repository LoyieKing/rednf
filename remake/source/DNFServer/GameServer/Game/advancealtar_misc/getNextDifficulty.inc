StageDifficulty::T StageDifficulty::getNextDifficulty(StageDifficulty::T diff)
{
    // P208/P163 + P46：switch 复现 ORIG 4.1 决策树（值缓存 %eax + test/cmp 链）。
    // 枚举底层为 unsigned 会吞 -1 case（「case 标号低于类型最小值」告警），
    // switch(int) 保留。b28d-35：case 必须按数值升序书写（P46 叶块排放序），
    // 原 0,1,-1 序使 4.4 产 `je` 反向树 + 叶序 [1,2,0]（ORIG 为 `jne` + 叶序 [0,1,2]）。
    switch (static_cast<int>(diff))
    {
    case -1:                             // -1 → 0
        return DIFF_EASY;
    case DIFF_EASY:                      // 0 → 1
        return DIFF_MEDIUM;
    case DIFF_MEDIUM:                    // 1 → 2
        return DIFF_HARD;
    default:
        return diff;
    }
