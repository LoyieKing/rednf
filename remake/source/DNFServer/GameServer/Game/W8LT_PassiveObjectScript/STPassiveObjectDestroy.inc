// ---- STPassiveObjectDestroy（ORIG C1 W 0x08a3d926 / D1 W 0x08a3e04c）----
// C1：5 × vector 构造（+0x08/+0x14/+0x20/+0x2c/+0x44）→ clear()（带 unwind 清理路径）；
// D1：空体，逆序析构 5 × vector（带 unwind 清理路径）。
STPassiveObjectDestroy::STPassiveObjectDestroy()
    : m_ints08(), m_strs14(), m_strs20(), m_ints2c(), m_ints44()
{
    clear();
}