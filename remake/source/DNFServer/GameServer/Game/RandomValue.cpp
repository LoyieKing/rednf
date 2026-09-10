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
// b28d-D42 verdict=DIFF [N86+N82] ORIG 0x085df826 orig/ours=75/78 ndiff=66。
// 残差为 vector begin/end 返回对象的 copy-elision 差（N86：4.1 恒临时槽+拷贝、
// 4.4 直构）+ 迭代器槽号连锁平移（-0x1c/-0x18/-0x14 vs -0x24/-0x20/-0x18）。
#include "RandomValue/operator.inc"
{
    int value = m_a;
    for (int i = 0; i < m_tryCount; ++i)
    {
        // [L27-r4 已实测] ORIG 段无具名 r 槽（我方多 `mov %eax,-0xc(%ebp)` + 一次 m_a 重载），
        //   但把 std::rand() 内联进表达式**反劣**（75→80：4.4 提前物化 m_a 到 %ebx 并
        //   增 push %esi / 帧 0x34→0x30）⇒ 维持具名 r（P329/P183 单向性族）。
        int r = std::rand();
        value = m_a + r % (m_b - m_a + 1);
        if (std::find(m_list.begin(), m_list.end(), value) == m_list.end())
            break;
    }
    return value;
}
