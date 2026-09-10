bool CCacheCharacterMgr::CacheCharacter(int characNo,
                                        CACHE_CHARACTER_TYPE* mem)
{
    Guard<Mutex> g(&m_mutex);
    std::pair<std::map<int, CACHE_CHARACTER_TYPE*>::iterator, bool> r =
        m_map.insert(std::make_pair(characNo, mem));
    if (!r.second)
        r.first->second = mem;   // 已存在则更新指针（ORIG 同形）
    return true;
