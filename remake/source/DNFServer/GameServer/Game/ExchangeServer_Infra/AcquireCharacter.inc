CACHE_CHARACTER_TYPE* CCacheCharacterMgr::AcquireCharacter(int characNo)
{
    Guard<Mutex> g(&m_mutex);
    CACHE_CHARACTER_TYPE* mem = 0;
    std::map<int, CACHE_CHARACTER_TYPE*>::iterator it = m_map.find(characNo);
    if (it != m_map.end())
    {
        mem = it->second;
        memset(mem, 0, sizeof(CACHE_CHARACTER_TYPE));
        mem->m_cacheTime = time(0);   // P217 同型直存（ORIG `mov -0xc,%edx; mov %eax,0x34880(%edx)`）
        return mem;
    }
    if (m_pool.Remain() == 0)
    {
        if (!CollectGarbage())
            return 0;
    }
    mem = m_pool.Acquire();
    // P225：ORIG `cmpl $0,-0xc; setne %al; test %al,%al; je` 为 4.1 bool 物化，
    // 4.4 对 `if` 恒折 cmpl/je ⇒ while+break 四段物化形复现。
    while (mem != 0)
    {
        memset(mem, 0, sizeof(CACHE_CHARACTER_TYPE));
        mem->m_cacheTime = time(0);   // P217 同型直存（同上）
        break;
    }
    return mem;
