// [b28d-solo-CClearReward] verdict=AE：循环体基址改数组指针形
// `(*(m_workProbs + drop))[i]`，复现 ORIG `lea 0x60(%eax),%ecx; …;
// lea (%ecx,%eax,1),%edx`（下标形 4.4 恒折 add 链；探针 /tmp/b28d_ccr/p5.cpp f1↔f2）。
int CClearReward::_getGenItemProbIdx(int value, eClearRewardDropProb_t drop)
{
    // [b28d-63] ORIG 0x08538694：基址 `this+drop*0xc+0x60` 每次重算（无具名
    // vector 引用局部，P0/P183）；循环体单次 operator[] 存 -0xc 后读两字段
    // （P157 例外）；尾部 size()<=i ⇒ mov $0xffffffff 岛。
    size_t i = 0;
    while (i < m_workProbs[drop].size())
    {
        // [b28d-solo-CClearReward] ORIG 0x085386a3 循环体基址形：
        // `mov 0x8(%ebp),%eax; lea 0x60(%eax),%ecx; …; lea (%ecx,%eax,1),%edx`
        // —— 数组名退化指针 + 下标（`*(m_workProbs+drop)`）才产「先算基址常量再
        // 合并索引」；下标形 `m_workProbs[drop][i]` 在 4.4 恒折 `add $0x60,%eax;
        // add this,%eax`（探针 /tmp/b28d_ccr/p5.cpp f1↔f2 实证）。
        const stGenItemProb_t& prob = (*(m_workProbs + drop))[i];
        if (value >= prob.v[0] && value <= prob.v[1])
            break;
        ++i;
    }
    if (m_workProbs[drop].size() <= i)
        return -1;
    return static_cast<int>(i);
}