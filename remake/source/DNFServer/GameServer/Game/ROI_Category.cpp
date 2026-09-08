// ============================================================================
// LinkHelper 核销批次 —— ROI_Category 实现（/tmp/undef_r21.txt：ROI_Category::_sort）。
// ORIG 证据：
//   _ZN12ROI_CategoryC1Ev   W @0x0822ac10 sz 0x1f：+0/+4/+8 三个 dword 清零。
//   _ZN12ROI_Category5_sortEv W @0x0822ac30 sz 0x10a：3 记录 (short key, byte mark)
//   冒泡排序，objdump 逐指令转写。
// ============================================================================

#include "ROI_Category.h"

ROI_Category::ROI_Category()
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
void ROI_Category::_sort()
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
