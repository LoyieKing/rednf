bool CCargo::set_cargo(void* data)
{
    if (m_capacity <= 0) return 0;
    m_sorted = 0;
    memcpy(m_pItems, data, 0x2438);
    _GetItemCount();
    return 1;
