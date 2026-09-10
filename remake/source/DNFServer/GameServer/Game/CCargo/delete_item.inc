bool CCargo::delete_item(int idx, int count, eItemDelReason reason)
{
    if (!IsOperatorable(idx))
        return 0;
    if (m_pItems[idx].m_addInfo == 0)
        return 0;
    if (count < 1)
        return 0;
    m_sorted = 0;
    // [b28d-solo-CCargo] ORIG 0x0850b794：`test %al,%al; je <非装备路径>`（正极性
    // + bool callee，无 xor/setne）⇒ 源码写 `if (isEquipable()) {_ResetSlot} else {…}`；
    // 装备路径块物理在前。
    if (m_pItems[idx].isEquipableItemType()) {
        _ResetSlot(idx, reason);
    } else {
        if (m_pItems[idx].m_addInfo2 == count) {
            _ResetSlot(idx, reason);
        } else {
            // [b28d-solo-CCargo] ORIG 无具名 cur 局部（P0/P48）：`mov 0x7(%eax),%eax;
            // sub count; test; jle` 两次独立重读 m_addInfo2。
            if (m_pItems[idx].m_addInfo2 - count <= 0)
                goto ZERO;
            m_pItems[idx].m_addInfo2 = m_pItems[idx].m_addInfo2 - count;
            m_pParent->m_historyLog.ItemDelCargo(
                m_pItems[idx].m_addInfo, count,
                m_pItems[idx].m_addInfo2, reason);
        }
    }
    goto ONE;
    // [b28d-solo-CCargo] ORIG 尾块序：`jle <+0x15d mov $0; jmp>` 早退块在
    // `+0x164 mov $1` 之前 ⇒ goto 尾标签串（P223；平铺 return 会被 4.4 就地物化）。
ZERO:
    return 0;
ONE:
    return 1;
