// ===========================================================================
// _GetAssaultPlace(unsigned int, int) —— ORIG T 0x082eda94 (0xd6)
// map 未命中抛 ENUM_ERROR(0x15)；authenValue 不匹配抛 ENUM_ERROR(0x8)。
// ===========================================================================
CAssaultPlace* CAssaultMgr::_GetAssaultPlace(unsigned int id, int authenValue)
{
    std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.find(id);
    if (it == m_places.end())
    {
        throw static_cast<ENUM_ERROR>(0x15);
    }
    CAssaultPlace* place = it->second;
    if (place->GetAuthenValue() != authenValue)
    {
        throw static_cast<ENUM_ERROR>(0x8);
    }
    return place;
}