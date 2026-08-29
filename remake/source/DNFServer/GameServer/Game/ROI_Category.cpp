// ============================================================================
// LinkHelper 核销批次 —— ROI_Category 实现（/tmp/undef_r21.txt：ROI_Category::_sort）。
// ORIG 证据：
//   _ZN12ROI_CategoryC1Ev   W @0x0822ac10 sz 0x1f：+0/+4/+8 三个 dword 清零。
//   _ZN12ROI_Category5_sortEv W @0x0822ac30 sz 0x10a：3 记录 (ushort key, byte mark)
//   冒泡排序，objdump 逐指令转写。
// ============================================================================

#include "ROI_Category.h"

ROI_Category::ROI_Category()
{
    // ORIG 0x0822ac10: movl $0x0,(%eax) / 0x4(%eax) / 0x8(%eax) —— 三个 dword 清零。
    unsigned int* dw = reinterpret_cast<unsigned int*>(this);
    dw[0] = 0;
    dw[1] = 0;
    dw[2] = 0;
}

// ORIG 0x0822ac30：记录 i 的 key=m_keys[i]，mark=m_marks[i]。
// 三次比较交换：if(k0>k1){swap k0,k1 & m0,m1}
//               if(k1>k2){swap k1,k2 & m1,m2}
//               if(k0>k1){swap k0,k1 & m0,m1}
void ROI_Category::_sort()
{
    if (m_keys[0] > m_keys[1])
    {
        unsigned short tmpKey = m_keys[0];       // -0x4(%ebp)
        m_keys[0] = m_keys[1];
        m_keys[1] = tmpKey;
        unsigned char tmpMark = m_marks[0];      // -0x1(%ebp)
        m_marks[0] = m_marks[1];
        m_marks[1] = tmpMark;
    }
    if (m_keys[1] > m_keys[2])
    {
        unsigned short tmpKey = m_keys[1];       // -0x4(%ebp)
        m_keys[1] = m_keys[2];
        m_keys[2] = tmpKey;
        unsigned char tmpMark = m_marks[1];      // -0x1(%ebp)
        m_marks[1] = m_marks[2];
        m_marks[2] = tmpMark;
    }
    if (m_keys[0] > m_keys[1])
    {
        unsigned short tmpKey = m_keys[0];       // -0x4(%ebp)
        m_keys[0] = m_keys[1];
        m_keys[1] = tmpKey;
        unsigned char tmpMark = m_marks[0];      // -0x1(%ebp)
        m_marks[0] = m_marks[1];
        m_marks[1] = tmpMark;
    }
}
