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
{
}

#include "W9Interval/Interval_1.inc"
{
    m_start = start;
    m_size = size;
}

// [b28d-Main 代落·S329 P317] 用户空析构定义删除（声明端同步）：隐式 trivial dtor 的
// D1/D2 仍由 new_allocator::destroy 发射 W @0x08577554；`_Destroy_aux<true>` 与 ORIG 同形。

#include "W9Interval/IntervalSet.inc"
    : m_intervals()
{
}

#include "W9Interval/dtor_IntervalSet.inc"
{
}

IntervalSet& IntervalSet::operator=(const IntervalSet& rhs) // W @0x08575ec4
{
    m_intervals = rhs.m_intervals;                     // ORIG: vector<Interval>::operator=
    return *this;
}

#include "W9Interval/Clear.inc"
{
    m_intervals.clear();                               // ORIG: vector<Interval>::clear()
}

#include "W9Interval/AddInterval.inc"
{
    m_intervals.push_back(interval);                   // ORIG: push_back(const&)
}

#include "W9Interval/Sort.inc"
{
    // ORIG: 空 set 直接跳过；否则 qsort(begin, size, sizeof(Interval), 比较器)
    // ORIG 0x085735e4：`call size; test %eax,%eax; sete %al; test %al,%al; jne`
    // ⇒ 源形为 size()==0 的条件物化（empty() 会 call empty 且无 sete，P194）。
    if (m_intervals.size() == 0)
        return;
    qsort(&m_intervals[0], m_intervals.size(), sizeof(Interval),
          &CompareIntervalForSort);
}

#include "W9Interval/GetTotalIntervalSize.inc"
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
}

#include "W9Interval/IsOverlappedInterval.inc"
{
    // ORIG: (aStart+aSize > bStart) && (bStart+bSize > aStart)
    if (aStart + aSize <= bStart)
        return false;
    if (bStart + bSize <= aStart)
        return false;
    return true;
}

#include "W9Interval/IntersectInterval.inc"
{
    // [b28d-lane15] ORIG 全寄存器、零局部槽（无 lhsEnd/rhsEnd 具名局部，P0/P183）：
    // 每个使用点重算 `start+size`；两个 early-return 独立（P133），两分支各带内层
    // `if (rhs.start < lhs.start+lhs.size)`（P225 拆嵌套 if，禁 && 合并）。
    // ORIG 两条件共享同一 FAIL 块（+0x2f `mov $0,%eax`）⇒ 合并短路（P133 反向用例：
    // 此处 ORIG 失败块唯一，独立 if 会生两个 FAIL 块各 +2 条）。
    if (lhs.m_start + lhs.m_size <= rhs.m_start ||
        rhs.m_start + rhs.m_size <= lhs.m_start)
        return false;

    // ORIG 两分支内层假路都落同一 ELSE1（+0xc6）⇒ 原码是 && 合并的 else-if 链
    // （4.4 对 `a && b` 直跳不物化，P225 边界）。
    if (lhs.m_start <= rhs.m_start &&
        rhs.m_start < lhs.m_start + lhs.m_size)
    {
        result->m_start = rhs.m_start;                  // 左端取 max(start)
        // ORIG 0x085730e4 `jge +0xa7`：真臂（rhsEnd-start）在远处、假臂紧跟
        // ⇒ 条件取反 + 两臂交换（P47/P59 块排布）。
        if (lhs.m_start + lhs.m_size < rhs.m_start + rhs.m_size)
            result->m_size = lhs.m_start + lhs.m_size - result->m_start;
        else
            result->m_size = rhs.m_start + rhs.m_size - result->m_start;
    }
    else if (rhs.m_start <= lhs.m_start &&
             lhs.m_start < rhs.m_start + rhs.m_size)
    {
        result->m_start = lhs.m_start;
        // ORIG 0x08573171 `jge +0x131`：真臂（lhsEnd-start）在远处 ⇒ 同法取反。
        if (rhs.m_start + rhs.m_size < lhs.m_start + lhs.m_size)
            result->m_size = rhs.m_start + rhs.m_size - result->m_start;
        else
            result->m_size = lhs.m_start + lhs.m_size - result->m_start;
    }
    // 未命中重叠窗口时 ORIG 落到 ret 1 且不写 result；此处与之一致。
    return true;
}

#include "W9Interval/IntersectIntervalSet.inc"
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
}

#include "W9Interval/MinusInterval.inc"
{
    // ORIG 控制流：
    //   遍历元素 iv，r = intersect(iv, sub)；不相交 → ++
    //   完全覆盖（r==iv）→ erase(it)，erase 返回值作新 it，直接回到 end 判断（无 ++）
    //   r.start==iv.start       → 当前槽改 {r.end, iv.end-r.end}，++
    //   r.end==iv.end           → 当前槽改 {iv.start, r.start-iv.start}，++
    //   中挖                    → 头部残留写回当前槽，++ 后 insert 尾部残留，再 ++
    // [P0 槽-局部双向对应律] ORIG 函数头序：`__normal_iterator()`@0x085760d4 默认构造
    // it(-0x14) → `Interval::Interval()`@0x08575e58 ×3（-0x1c/-0x24/-0x2c）→
    // `begin()`@0x085760e2 结果拷入 it。⇒ it/ret/temp/iv 四个具名局部均声明在**函数
    // 作用域**（循环内每次迭代不再构造），循环内 `iv = *it` 是 trivial 拷贝赋值。
    // [单一 temp 复用] 三个分支同用 -0x24/-0x20 槽写新区间：分支2 写 {r.end, iv.end-r.end}，
    // 分支3/中挖 写 {iv.start, r.start-iv.start}，中挖再覆盖为尾部残留 {r.end, iv.end-r.end}
    // 并直接作 `insert` 的源（`insert(...,&local_24)`@0x08573592）⇒ 原码只有一个临时
    // Interval 具名局部。
    // [丢弃写入判据] ORIG 中挖分支：`operator++(int)`@0x08576248（后置，返回值入 -0xc 后
    // 丢弃）→ `insert`@0x0857627c → **`mov -0x3c,%eax; mov %eax,-0x14`**（it = insert 返回
    // 迭代器，0x0857359a）→ 外层 `operator++()`@0x085763aa。原码为
    // `it++; it = m_intervals.insert(it, temp);`——旧码丢 insert 返回值 ⇒ it 失效后仍自增。
    // [b28d-lane15 判词·N（P71 空析构省略跨版本差）] ORIG 0x08573394 对 iv/ret/tail
    // 三个 Interval 局部**不调 D1**（尾部 +0x246 leave/ret 直落），4.4 恒调 3 处
    // D1 + 布 EH cleanup（223/202 形均试，ORIG 183）。ORIG 的 D1 符号仅被
    // `new_allocator<Interval>::destroy` 引用（0x08577566，全仓唯一调用点）；删用户
    // 析构会连带丢该 W 符号与 destroy 实例形态 ⇒ 无源码解，封顶。
    // 另：函数头四对象槽序 -0x14/-0x1c/-0x24/-0x2c 与 ORIG 一致，残差全为析构调用。
    std::vector<Interval>::iterator it;
    Interval ret;
    Interval temp;
    Interval iv;
    it = m_intervals.begin();
    while (it != m_intervals.end())
    {
        // ORIG 逐字段两次 `operator*`@0x08576158（085733ec/085733f6）⇒ 原码为两个
        // `(*it)` 表达式（`iv = *it` 只会解引用一次，且 4.4 的 `it->` 走 operator->）。
        iv.m_start = (*it).m_start;
        iv.m_size = (*it).m_size;
        // ORIG `test %al,%al; je 085735a0`（不相交直接落 ++it）⇒ 原码为
        // `if (IntersectInterval(...)) { ...三种情况... }` 包裹式，非 early-continue。
        if (IntersectInterval(&ret, iv, interval))
        {
            if (ret.m_start == iv.m_start && ret.m_size == iv.m_size)
            {
                it = m_intervals.erase(it);             // 完全覆盖 → 无 ++
                continue;
            }

            if (ret.m_start == iv.m_start)
            {
                temp.m_start = ret.m_start + ret.m_size;
                temp.m_size = (iv.m_start + iv.m_size) - temp.m_start;
                (*it).m_start = temp.m_start;
                (*it).m_size = temp.m_size;
            }
            else if (ret.m_start + ret.m_size == iv.m_start + iv.m_size)
            {
                temp.m_start = iv.m_start;
                temp.m_size = ret.m_start - temp.m_start; // ORIG 085734cf 用 temp.start
                (*it).m_start = temp.m_start;
                (*it).m_size = temp.m_size;
            }
            else
            {
                temp.m_start = iv.m_start;
                temp.m_size = ret.m_start - temp.m_start; // ORIG 0857350a 用 temp.start
                (*it).m_start = temp.m_start;
                (*it).m_size = temp.m_size;

                temp.m_start = ret.m_start + ret.m_size;  // 尾部残留（覆盖 temp）
                temp.m_size = (iv.m_start + iv.m_size) - temp.m_start;
                it++;                                     // ORIG: 后置 ++（0x08576248）
                it = m_intervals.insert(it, temp);        // ORIG: 返回值回写 it（0x0857359a）
            }
        }
        ++it;
    }
}
