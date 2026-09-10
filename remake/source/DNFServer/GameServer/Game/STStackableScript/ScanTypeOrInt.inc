int ScanTypeOrInt(bool& isType, std::string& s, bool& isInt, bool consume);

// ---- stStackableBooster_t::clear（ORIG W 0x089c3076：
//      vector::clear(@+0x00) + map::clear(@+0x0c)）----
__attribute__((weak)) void stStackableBooster_t::clear()
{
    m_elements.clear();
    m_map.clear();
