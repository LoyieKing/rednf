// W9Interval.cpp — Wave9 Interval 域权威实现
// ORIG 证据：docs/class_func_reports/IntervalSet/* + objdump df_game_r。
// 各函数逐指令转写自汇编（地址见注释）。

#include "Interval.h"

#include <stdlib.h>

// ORIG t @0x08572eac _ZL22CompareIntervalForSortPKvS0_（文件内 static）：
//   lhs.start < rhs.start → -1；相等 → 0；大于 → 1。
static int CompareIntervalForSort(const void* lhs, const void* rhs)
{
    // P0：ORIG 槽存的是指针（-0x8/-0x4），使用点解引用 → 指针局部非值局部。
    const long* lp = static_cast<const long*>(lhs);
    const long* rp = static_cast<const long*>(rhs);
    if (*lp < *rp)
        return -1;
    if (*lp == *rp)
        return 0;
    return 1;
}

Interval::Interval()                                   // W @0x08575e58（空体）
{
}

Interval::Interval(long start, long size)              // W @0x08575e5e
{
    m_start = start;
    m_size = size;
}

Interval::~Interval()                                  // W @0x08577554（空体）
{
}

IntervalSet::IntervalSet()                             // W @0x08575e9c：仅构造 vector<Interval>
    : m_intervals()
{
}

IntervalSet::~IntervalSet()                            // W @0x08575eb0：仅析构 vector
{
}

IntervalSet& IntervalSet::operator=(const IntervalSet& rhs) // W @0x08575ec4
{
    m_intervals = rhs.m_intervals;                     // ORIG: vector<Interval>::operator=
    return *this;
}

void IntervalSet::Clear()                              // T @0x08573366
{
    m_intervals.clear();                               // ORIG: vector<Interval>::clear()
}

void IntervalSet::AddInterval(const Interval& interval) // T @0x0857337a
{
    m_intervals.push_back(interval);                   // ORIG: push_back(const&)
}

void IntervalSet::Sort()                               // T @0x085735dc
{
    // ORIG: 空 set 直接跳过；否则 qsort(begin, size, sizeof(Interval), 比较器)
    if (m_intervals.empty())
        return;
    qsort(&m_intervals[0], m_intervals.size(), sizeof(Interval),
          &CompareIntervalForSort);
}

long IntervalSet::GetTotalIntervalSize()               // T @0x0857363c
{
    // ORIG: 遍历累加元素 +0x4（m_size）
    long total = 0;
    std::vector<Interval>::iterator it = m_intervals.begin();
    std::vector<Interval>::iterator end = m_intervals.end();
    for (; it != end; ++it)
        total += it->m_size;
    return total;
}

bool IsOverlappedInterval(long aStart, long aSize, long bStart, long bSize) // T @0x0857302c
{
    // ORIG: (aStart+aSize > bStart) && (bStart+bSize > aStart)
    if (aStart + aSize <= bStart)
        return false;
    if (bStart + bSize <= aStart)
        return false;
    return true;
}

bool IntersectInterval(Interval* result, const Interval& lhs, const Interval& rhs) // T @0x08573060
{
    const long lhsEnd = lhs.m_start + lhs.m_size;
    const long rhsEnd = rhs.m_start + rhs.m_size;

    if (!(lhsEnd > rhs.m_start) || !(rhsEnd > lhs.m_start))
        return false;                                   // 不相交 → ret 0

    if (lhs.m_start <= rhs.m_start && rhs.m_start < lhsEnd)
    {
        result->m_start = rhs.m_start;                  // 左端取 max(start)
        if (lhsEnd >= rhsEnd)
            result->m_size = rhsEnd - result->m_start;
        else
            result->m_size = lhsEnd - result->m_start;
    }
    else if (rhs.m_start <= lhs.m_start && lhs.m_start < rhsEnd)
    {
        result->m_start = lhs.m_start;
        if (rhsEnd >= lhsEnd)
            result->m_size = lhsEnd - result->m_start;
        else
            result->m_size = rhsEnd - result->m_start;
    }
    // 未命中重叠窗口时 ORIG 落到 ret 1 且不写 result；此处与之一致。
    return true;
}

void IntersectIntervalSet(IntervalSet* out, const IntervalSet& lhs, const IntervalSet& rhs) // T @0x085731b2
{
    // ORIG: out->Clear(); 双重遍历：IntersectInterval 命中则 AddInterval(交集)
    out->Clear();
    for (std::vector<Interval>::const_iterator ai = lhs.m_intervals.begin();
         ai != lhs.m_intervals.end(); ++ai)
    {
        for (std::vector<Interval>::const_iterator bi = rhs.m_intervals.begin();
             bi != rhs.m_intervals.end(); ++bi)
        {
            Interval cross;
            if (IntersectInterval(&cross, *ai, *bi))
                out->AddInterval(cross);
        }
    }
}

void IntervalSet::MinusInterval(const Interval& interval) // T @0x08573394
{
    // ORIG 控制流：
    //   遍历元素 iv，r = intersect(iv, sub)；不相交 → ++
    //   完全覆盖（r==iv）→ erase(it)，erase 返回值作新 it，直接回到 end 判断（无 ++）
    //   r.start==iv.start       → 当前槽改 {r.end, iv.end-r.end}，++
    //   r.end==iv.end           → 当前槽改 {iv.start, r.start-iv.start}，++
    //   中挖                    → 头部残留写回当前槽，++ 后 insert 尾部残留，再 ++
    std::vector<Interval>::iterator it = m_intervals.begin();
    while (it != m_intervals.end())
    {
        const Interval iv = *it;
        Interval ret;
        if (!IntersectInterval(&ret, iv, interval))
        {
            ++it;
            continue;
        }

        if (ret.m_start == iv.m_start && ret.m_size == iv.m_size)
        {
            it = m_intervals.erase(it);                 // 完全覆盖 → 无 ++
            continue;
        }

        if (ret.m_start == iv.m_start)
        {
            const long leftEnd = ret.m_start + ret.m_size;
            it->m_start = leftEnd;
            it->m_size = (iv.m_start + iv.m_size) - leftEnd;
        }
        else if (ret.m_start + ret.m_size == iv.m_start + iv.m_size)
        {
            it->m_start = iv.m_start;
            it->m_size = ret.m_start - iv.m_start;
        }
        else
        {
            const long tailStart = ret.m_start + ret.m_size;
            it->m_start = iv.m_start;
            it->m_size = ret.m_start - iv.m_start;

            Interval tail(tailStart, (iv.m_start + iv.m_size) - tailStart);
            ++it;
            m_intervals.insert(it, tail);               // ORIG: insert 后取返回迭代器再 ++
        }
        ++it;
    }
}
