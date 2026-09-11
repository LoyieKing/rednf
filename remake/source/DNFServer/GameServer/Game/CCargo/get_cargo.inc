// ---- get_cargo @ 0x850b164 ----
bool CCargo::get_cargo(void* out) const
{
    if (m_capacity <= 0) return 0;
    memcpy(out, m_pItems, 0x2438);
    return 1;
}