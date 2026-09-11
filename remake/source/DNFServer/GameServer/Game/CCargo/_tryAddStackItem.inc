// [b28d-solo-CCargo] AE（fresh 65=65；compare_tu clean 口径）——尾块序翻正：
// ORIG `jne <ret -1 块，物理最末直落 epilogue>` + `mov 0x10(%ebp),%eax; jmp` 在前
// ⇒ 源码 `if (==) {…; return slot;} return -1;`（P222 正向包裹）。原 N18b 判词作废。
int CCargo::_tryAddStackItem(const Inven_Item& item, int slot) const
{
    if (m_pItems[slot].m_addInfo == item.m_addInfo) {
        if (0x7fffffff - item.get_add_info() < m_pItems[slot].m_addInfo2) {
            return -1;
        }
        // [b28d-P226] 同 _AddStackItem：内联实参保留首值 callee-saved 形。
        if (!checkStackableLimit((unsigned long)m_pItems[slot].m_addInfo,
                                 m_pItems[slot].get_add_info()
                                     + item.get_add_info()))
            return -10;
        return slot;
    }
    return -1;
}