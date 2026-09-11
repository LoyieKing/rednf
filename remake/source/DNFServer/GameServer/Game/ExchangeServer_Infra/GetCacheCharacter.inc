// GetCacheCharacter ORIG 0x8490cee：查 map，erase 可选。
bool CCacheCharacterMgr::GetCacheCharacter(int characNo,
                                           CACHE_CHARACTER_TYPE** out,
                                           bool erase)
{
    Guard<Mutex> g(&m_mutex);
    std::map<int, CACHE_CHARACTER_TYPE*>::iterator it = m_map.find(characNo);
    if (it != m_map.end())
    {
        *out = it->second;
        if (erase)
            m_map.erase(it);
        return true;
    }
    return false;
}