// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W9Interval/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "W9Interval/Interval.inc"


#include "W9Interval/Interval_1.inc"


// [b28d-Main 代落·S329 P317] 用户空析构定义删除（声明端同步）：隐式 trivial dtor 的
// D1/D2 仍由 new_allocator::destroy 发射 W @0x08577554；`_Destroy_aux<true>` 与 ORIG 同形。

#include "W9Interval/IntervalSet.inc"


#include "W9Interval/dtor_IntervalSet.inc"


IntervalSet& IntervalSet::operator=(const IntervalSet& rhs) // W @0x08575ec4
{
    m_intervals = rhs.m_intervals;                     // ORIG: vector<Interval>::operator=
    return *this;
}

#include "W9Interval/Clear.inc"


#include "W9Interval/AddInterval.inc"


#include "W9Interval/Sort.inc"


#include "W9Interval/GetTotalIntervalSize.inc"


#include "W9Interval/IsOverlappedInterval.inc"


#include "W9Interval/IntersectInterval.inc"


#include "W9Interval/IntersectIntervalSet.inc"


#include "W9Interval/MinusInterval.inc"

