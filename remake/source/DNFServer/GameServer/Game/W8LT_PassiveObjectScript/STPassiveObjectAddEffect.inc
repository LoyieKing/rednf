// ---- STPassiveObjectAddEffect（ORIG C1 W 0x08a3da0c / D1 W 0x08a3e110）----
// C1：2 × vector 构造（+0x00 string/+0x0c int）→ clear()（带 unwind 清理路径）；
// D1：空体，逆序析构 2 × vector（带 unwind 清理路径）。
STPassiveObjectAddEffect::STPassiveObjectAddEffect()
    : m_strs00(), m_ints0c()
{
    clear();
}