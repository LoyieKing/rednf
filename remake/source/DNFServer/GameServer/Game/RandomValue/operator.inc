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
int CRandomValue::operator()()
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