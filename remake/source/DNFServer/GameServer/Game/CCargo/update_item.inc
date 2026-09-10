void CCargo::update_item(int idx, const Inven_Item& item)
{
    if (!IsOperatorable(idx))
        return;
    if (m_pItems[idx].m_addInfo == 0) {
        _AddItem(item, idx);
    } else {
        // [b28d-N84] 同 _AddItem：ORIG 逐 dword 拷贝（0x0850bb.. 段）非 memcpy。
        m_pItems[idx] = item;
    }
