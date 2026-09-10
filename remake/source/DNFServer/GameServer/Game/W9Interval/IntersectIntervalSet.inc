void IntersectIntervalSet(IntervalSet* out, const IntervalSet& lhs, const IntervalSet& rhs) // T @0x085731b2
{
    // [b28d-lane15] ORIG 槽：-0x14 ai / -0x18 bi / -0x20 cross 三个对象在函数头
    // **先默认构造**（+0x07/+0x12/+0x1d 三次 C1），再 out->Clear()；迭代器一律
    // 先声明后 `= begin()`（P50反例复活），end() 每轮重算（不缓存，P13 不适用）。
    // [b28d-lane15 判词·N（P71 空析构省略跨版本差）] 上述槽序/调用序已与 ORIG 对齐；
    // 残差唯一来源 = cross 的析构：ORIG 0x085731b2 尾部 `mov -0x4(%ebp),%ebx;
    // leave; ret` 直落**不调 D1、无 EH cleanup**，4.4 恒调 2 处 D1（正常+unwind）
    // 并多 push %esi。ORIG 的 D1 W 符号全仓唯一调用点是
    // `new_allocator<Interval>::destroy`（0x08577566）；删用户析构会连带丢该符号
    // ⇒ 无源码解，封顶。
    std::vector<Interval>::const_iterator ai;
    std::vector<Interval>::const_iterator bi;
    Interval cross;
    out->Clear();
    for (ai = lhs.m_intervals.begin(); ai != lhs.m_intervals.end(); ++ai)
    {
        for (bi = rhs.m_intervals.begin(); bi != rhs.m_intervals.end(); ++bi)
        {
            if (IntersectInterval(&cross, *ai, *bi))
                out->AddInterval(cross);
        }
    }
