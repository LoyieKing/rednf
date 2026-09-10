void CCargo::sort()
{
    if (!m_sorted) {
        qsort(m_pItems, 0x98, sizeof(Inven_Item), _CompareSlot);
        m_sorted = 1;
    }
