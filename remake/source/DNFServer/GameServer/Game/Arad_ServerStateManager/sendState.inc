void Arad_ServerStateManager::sendState(CUser* user)
{
    Guard<Mutex> guard(&m_lock);
    std::for_each(m_stateMap.begin(), m_stateMap.end(), SSendEffect(user));
