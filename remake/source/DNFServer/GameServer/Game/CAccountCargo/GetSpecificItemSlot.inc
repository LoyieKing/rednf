int CAccountCargo::GetSpecificItemSlot(int itemIdx)
{
    // [b28d-solo-CAccountCargo 判词·AE] ORIG 0x0828a61a：29/29 全等；同 GetEmptySlot
    // 的 i486 路由消 cmova（P230）。p167 CLEAR（0 call）。
    int limit = (int)(m_capacity > 0x38 ? 0x38 : m_capacity);
    for (int i = 0; i < limit; ++i) {
        if (itemIdx == m_slots[i].m_addInfo)
            return i;
    }
    return -1;
