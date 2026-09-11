// ORIG _ZN12CRandomValueC1ERSt6vectorIiSaIiEEiii（0x085df7fe，0x28 字节）：
// +0x00=vector& 地址、+0x04=第4参、+0x08=第3参、+0x0c=第5参。
CRandomValue::CRandomValue(std::vector<int>& list, int a, int b, int c)
    : m_list(list), m_b(b), m_a(a), m_tryCount(c)
{
}