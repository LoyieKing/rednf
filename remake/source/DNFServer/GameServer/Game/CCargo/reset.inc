// ---- reset @ 0x850b0c2 ----
void CCargo::reset()
{
    m_itemCount = 0;
    m_pParent = 0;
    m_capacity = 0;
    m_sorted = 0;
    if (m_pItems != 0)
        memset(m_pItems, 0, 0x2438);
}