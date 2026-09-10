long IntervalSet::GetTotalIntervalSize()               // T @0x0857363c
{
    // ORIG: 遍历累加元素 +0x4（m_size）。槽布局 -0x14(it 默认构造) / -0xc(total)
    // / -0x1c(begin 临时) / -0x10(end 每轮重算)。
    // [b28d-lane15] P28+P50反例复活：it 必须先默认构造再 `it = begin()`（经 -0x1c
    // 临时中转）；end() 不得缓存（P13 不适用），循环条件逐轮调用 end()。
    std::vector<Interval>::iterator it;
    long total = 0;
    it = m_intervals.begin();
    for (; it != m_intervals.end(); ++it)
        total += it->m_size;
    return total;
