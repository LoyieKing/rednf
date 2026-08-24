// CRandomValue（从 GameStubs.cpp 桩迁移后按 ORIG 反汇编真实实现）。
#include "RandomValue.h"
#include <algorithm>
#include <cstdlib>

// ORIG _ZN12CRandomValueC1ERSt6vectorIiSaIiEEiii（0x085df7fe，0x28 字节）：
// +0x00=vector& 地址、+0x04=第4参、+0x08=第3参、+0x0c=第5参。
CRandomValue::CRandomValue(std::vector<int>& list, int a, int b, int c)
    : m_list(list), m_b(b), m_a(a), m_tryCount(c)
{
}

// ORIG _ZN12CRandomValueclEv（0x085df826，0xea 字节）：
// value 初值 = m_a(+0x8)；循环至多 m_tryCount 次：
//   r = rand()；value = m_a + r % (m_b - m_a + 1)；
//   若 value 不在排除列表 m_list 中则提前返回。
// 循环耗尽仍返回最后一次 value。
int CRandomValue::operator()()
{
    int value = m_a;
    for (int i = 0; i < m_tryCount; ++i)
    {
        int r = std::rand();
        value = m_a + r % (m_b - m_a + 1);
        if (std::find(m_list.begin(), m_list.end(), value) == m_list.end())
            break;
    }
    return value;
}
