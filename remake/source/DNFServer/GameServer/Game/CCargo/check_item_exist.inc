int CCargo::check_item_exist(int itemIdx) const
{
    for (int i = 0; i <= 0x97; ++i) {
        if (m_pItems[i].m_addInfo == itemIdx)
            return i;
    }
    return -1;
