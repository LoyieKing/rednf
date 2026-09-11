// ---------------------------------------------------------------------------
// sendState(CUser*) —— ORIG 0x081a9500：
// Guard 内以 SSendEffect 遍历全 map，逐条 notifyOpenMessageDialog(user,1,code,msg)。
// ORIG 用 std::for_each 驱动（弱实例 _ZSt8for_each…SSendEffect @0x81a9ee5）。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::sendState(CUser* user)
{
    Guard<Mutex> guard(&m_lock);
    std::for_each(m_stateMap.begin(), m_stateMap.end(), SSendEffect(user));
}