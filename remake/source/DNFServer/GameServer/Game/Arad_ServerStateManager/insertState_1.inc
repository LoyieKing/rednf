void Arad_ServerStateManager::insertState(ENUM_GLOBAL_EFFECT_ARAD code,
                                          const AradServerStateMessage& msg)
{
    Guard<Mutex> guard(&m_lock);
    insertStateNoSync(code, msg);
