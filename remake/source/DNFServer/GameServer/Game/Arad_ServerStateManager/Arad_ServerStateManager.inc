// ---------------------------------------------------------------------------
// ctor —— ORIG 0x081a922c：*this 置 0（m_flag=0）、Mutex 构造、map 构造。
// ---------------------------------------------------------------------------
Arad_ServerStateManager::Arad_ServerStateManager()
    : m_flag(0), m_lock(), m_stateMap()
{
}