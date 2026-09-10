// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ROI_Category/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "ROI_Category.h"

#include "ROI_Category/ROI_Category.inc"
{
    // ORIG 0x0822ac10: 前两 movl 共享基址（8 字节整块），第三重读 this —— long long 块清 + 第三 dword
    *(long long*)this = 0;
    m_dw8 = 0;                       // ORIG movl $0,0x8(%eax)（disp 折叠）
}

// ORIG 0x0822ac30：记录 i 的 key=m_keys[i]，mark=m_marks[i]。
// 三次比较交换：if(k0>k1){swap k0,k1 & m0,m1}
//               if(k1>k2){swap k1,k2 & m1,m2}
//               if(k0>k1){swap k0,k1 & m0,m1}
// short 字段直比（无 static_cast）复现 ORIG `movzwl (%eax),%edx` 一步直落 +
// jle 有符号比较（b28c-lane25 探针 /tmp/l25probe/roi.cpp fA/fB 实证）。
#include "ROI_Category/_sort.inc"
{
    // tmpKey 与字段同型（short）：P211 同型 ⇒ 4.4 store LHS(this) 先，
    // 复现 ORIG `mov this; movzwl -0x4; mov %dx,0x2` 序（异型恒 RHS 先）。
    short tmpKey;
    unsigned char tmpMark;
    if (m_keys[0] > m_keys[1]) // ORIG jle 有符号比较
    {
        tmpKey = m_keys[0];                       // ORIG 共用 -0x4(%ebp)
        m_keys[0] = m_keys[1];
        m_keys[1] = tmpKey;
        tmpMark = m_marks[0];                     // ORIG 共用 -0x1(%ebp)
        m_marks[0] = m_marks[1];
        m_marks[1] = tmpMark;
    }
    if (m_keys[1] > m_keys[2]) // ORIG jle 有符号比较
    {
        tmpKey = m_keys[1];
        m_keys[1] = m_keys[2];
        m_keys[2] = tmpKey;
        tmpMark = m_marks[1];
        m_marks[1] = m_marks[2];
        m_marks[2] = tmpMark;
    }
    if (m_keys[0] > m_keys[1]) // ORIG jle 有符号比较
    {
        tmpKey = m_keys[0];
        m_keys[0] = m_keys[1];
        m_keys[1] = tmpKey;
        tmpMark = m_marks[0];
        m_marks[0] = m_marks[1];
        m_marks[1] = tmpMark;
    }
}
