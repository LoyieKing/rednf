// ============================================================================
// DisJointResult ctor @ 0x082347a2：循环 i=0..4 将两组 5 int 清零
// （等价整体清零 10 个 int）。
// [b28d-solo-ItemUpgradeVtableB2] 旧判 N4（P30 add-vs-lea）已翻正：ORIG 第二组
//   `add $0x4,%edx; movl $0,0x4(%eax,%edx,4)` 的偏移 0x14 = 索引 add 4（16B）
//   + disp 4，只在**独立成员数组**（m_b@+0x14）下标形下由 4.4 复现（探针 c5/c7）；
//   单数组 m_f[i+5] 恒折 `lea 5(%eax)`（c1）。DisJoint.h 已加 union 双视图
//   m_f[10] / {m_a[5], m_b[5]}（sizeof 0x28、全偏移不变）。
// ============================================================================
DisJointResult::DisJointResult()
{
    for (int i = 0; i <= 4; ++i)
    {
        m_a[i] = 0;
        m_b[i] = 0;
    }
}