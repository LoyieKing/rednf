int CCargo::_GetItemCount()
{
    m_itemCount = 0;
    for (int i = 0; i < 0x98; ++i) {
        if (m_pItems[i].m_addInfo != 0)
            ++m_itemCount;
    }
    return m_itemCount;
