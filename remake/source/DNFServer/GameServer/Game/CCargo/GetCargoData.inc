int CCargo::GetCargoData(int itemIdx, Inven_Item& out) const
{
    for (int i = 0; i <= 0x97; ++i) {
        if (m_pItems[i].m_addInfo == itemIdx) {
            out = m_pItems[i];   // [b28d-N84] ORIG 逐 dword 拷贝非 memcpy
            return i;
        }
    }
    return -1;
