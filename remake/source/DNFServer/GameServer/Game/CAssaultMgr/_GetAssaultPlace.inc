CAssaultPlace* CAssaultMgr::_GetAssaultPlace(unsigned int id)
{
    // ORIG 0x82ed9fe：未命中抛 ENUM_ERROR(0x15)。
    std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.find(id);
    if (it == m_places.end())
        throw (ENUM_ERROR)0x15;   // ORIG 抛 ENUM_ERROR(0x15)
    // ORIG 0x82eda3x：`mov 0x4(%eax),%eax; mov %eax,-0xc; mov -0xc,%eax`
    // ⇒ second 具名物化槽（技巧13 声明位置=落槽时机）。
    CAssaultPlace* v = it->second;
    return v;
