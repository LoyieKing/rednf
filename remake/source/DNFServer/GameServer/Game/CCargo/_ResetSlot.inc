// ---- _ResetSlot @ 0x850b1a0 ----
void CCargo::_ResetSlot(int idx, eItemDelReason reason)
{
    // [b28d-N84] ORIG 无默认 ctor call，直接逐 dword 拷贝 ⇒ 拷贝初始化（隐式拷贝
    // 构造内联），禁「默认构造 + 赋值」两段（多 3 条 ctor call）。
    Inven_Item saved = m_pItems[idx];
    m_pItems[idx].reset();
    if (saved.isEquipableItemType()) {
        m_pParent->m_historyLog.ItemDelCargo(saved.m_addInfo, 1, 0, reason);
    } else {
        m_pParent->m_historyLog.ItemDelCargo(saved.m_addInfo,
                                             saved.m_addInfo2, 0, reason);
    }
    --m_itemCount;
}