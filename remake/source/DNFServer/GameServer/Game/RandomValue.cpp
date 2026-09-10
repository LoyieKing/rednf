// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RandomValue/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CRandomValue（从 GameStubs.cpp 桩迁移后按 ORIG 反汇编真实实现）。
#include "RandomValue.h"
#include <algorithm>
#include <cstdlib>

// ORIG _ZN12CRandomValueC1ERSt6vectorIiSaIiEEiii（0x085df7fe，0x28 字节）：
// +0x00=vector& 地址、+0x04=第4参、+0x08=第3参、+0x0c=第5参。
#include "RandomValue/CRandomValue.inc"
    : m_list(list), m_b(b), m_a(a), m_tryCount(c)
{
}

// ORIG _ZN12CRandomValueclEv（0x085df826，0xea 字节）：
// value 初值 = m_a(+0x8)；循环至多 m_tryCount 次：
//   r = rand()；value = m_a + r % (m_b - m_a + 1)；
//   若 value 不在排除列表 m_list 中则提前返回。
// 循环耗尽仍返回最后一次 value。
// [P2-S4 2026-09-11] 交换加法项序后 gap 由 +3（我方多 3）→ 0（orig/ours=76/76）。
//   键：ORIG 0x085df861 `idiv %ecx` 之后才 `mov 0x8(%ebp),%eax; mov 0x8(%eax),%eax;
//   lea (%edx,%eax,1),%eax`（m_a 在除法后读取）；而 `m_a + r % (...)` 让 4.4 先把
//   m_a 物化到 %ecx 再算分母（多一次 this 重载）。改写为 `r % (...) + m_a` 后 4.4
//   与 ORIG 求值序一致。剩余残差：4.4 仍为 r 具名槽（ORIG 无）导致分母中转寄存器
//   用 %ecx/%eax 而非 %ebx（ORIG 有 push %ebx / 帧 0x34；我方 0x38），属寄存器分配。
//   ORIG 0x085df826 orig/ours=76/76（fdiff 75/75，10 块等量替换，全为槽号/寄存器差）。
#include "RandomValue/operator.inc"
{
    int value = m_a;
    for (int i = 0; i < m_tryCount; ++i)
    {
        // 具名 r 保留（ORIG 无 r 槽，但内联 std::rand() 进表达式会反劣：+5 条、
        // 多 push %esi、帧 0x34→0x30，见顶部 P2-S4 记录）。
        int r = std::rand();
        // 项序必须是 `r % (...) + m_a`（而非 `m_a + r % (...)`）：让 4.4 在 idiv
        // 之后才读 m_a，复现 ORIG 0x085df863 段的求值序（gap +3 → 0）。
        value = r % (m_b - m_a + 1) + m_a;
        if (std::find(m_list.begin(), m_list.end(), value) == m_list.end())
            break;
    }
    return value;
}
