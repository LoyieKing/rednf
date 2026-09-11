// AcquireCharacter ORIG 0x8490a8a：map 命中→memset+刷新 cacheTime 直接复用；
// 未命中且池空→CollectGarbage 回收，仍无则失败返回 0；否则池 Acquire+清零。
// [b28d-solo-ExchangeServer_Infra 判词·AE] 三路（命中直返 / 池空回收失败返 0 /
// 池 Acquire 清零）与 ORIG 逐条全等；池 Acquire 后 `while (mem != 0) {…; break;}`
// （P225）复现 ORIG 四段物化 `cmpl $0,-0xc; setne %al; test %al,%al; je`。
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
}