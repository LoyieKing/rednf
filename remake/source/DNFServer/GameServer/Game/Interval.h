#ifndef DNF_GAME_INTERVAL_H_
#define DNF_GAME_INTERVAL_H_

// ============================================================================
// Interval / IntervalSet 权威声明（Wave9 唯一 owner: Game/W9Interval.cpp）
//
// ORIG df_game_r 证据：
//   _ZN8IntervalC1Ell            W @0x08575e5e — start=arg1@+0, size=arg2@+4
//   _ZN8IntervalC1Ev             W @0x08575e58 — 空体（POD 不初始化）
//   _ZN8IntervalD1Ev             W @0x08577554 — 空体
//   _ZN11IntervalSetC1Ev         W @0x08575e9c — 仅构造 vector<Interval>
//   _ZN11IntervalSetD1Ev         W @0x08575eb0 — 析构 vector
//   _ZN11IntervalSetaSERKS_      W @0x08575ec4 — vector 赋值
//   Clear                T @0x08573366   AddInterval    T @0x0857337a
//   MinusInterval        T @0x08573394   Sort           T @0x085735dc
//   GetTotalIntervalSize T @0x0857363c
// 自由函数：
//   _Z20IsOverlappedIntervalllll                  T @0x0857302c
//   _Z17IntersectIntervalP8IntervalRKS_S2_        T @0x08573060
//   _Z20IntersectIntervalSetP11IntervalSetRKS_S2_ T @0x085731b2
// 布局：GetTotalIntervalSize 累加元素 +0x4 → size 在 +0x4，start 在 +0x0；
//       Sort 用 qsort 元素大小 0x8，CompareIntervalForSort 按 +0 比较。
// ============================================================================

#include <vector>
#include <stddef.h>

class Interval
{
public:
    long m_start; // +0x00
    long m_size;  // +0x04

    Interval();                                      // W @0x08575e58（空）
    Interval(long start, long size);                 // W @0x08575e5e
    ~Interval();                                     // W @0x08577554（空体；公开以支持 vector<Interval> 生命周期）
};

// 布局断言（C++98 兼容）：两 long = 0x8。
typedef char W9_IntervalSizeAssert[(sizeof(Interval) == 8) ? 1 : -1];

class IntervalSet
{
public:
    std::vector<Interval> m_intervals;               // ORIG 首成员 vector<Interval> @+0

    IntervalSet();                                   // W @0x08575e9c
    ~IntervalSet();                                  // W @0x08575eb0
    IntervalSet& operator=(const IntervalSet& rhs);  // W @0x08575ec4

    void Clear();                                    // T @0x08573366
    void AddInterval(const Interval& interval);      // T @0x0857337a
    void MinusInterval(const Interval& interval);    // T @0x08573394
    void Sort();                                     // T @0x085735dc
    long GetTotalIntervalSize();                     // T @0x0857363c（ORIG 非 const）
};


bool IsOverlappedInterval(long aStart, long aSize, long bStart, long bSize);            // T @0x0857302c
bool IntersectInterval(Interval* result, const Interval& lhs, const Interval& rhs);     // T @0x08573060
void IntersectIntervalSet(IntervalSet* out, const IntervalSet& lhs, const IntervalSet& rhs); // T @0x085731b2


#endif /* DNF_GAME_INTERVAL_H_ */
