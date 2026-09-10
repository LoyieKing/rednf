void Arad_ServerStateManager::insertState(arad::SigServerState* sig)
{
    Guard<Mutex> guard(&m_lock);
    for (int i = 0; i < sig->m_count; ++i)
    {
        insertStateNoSync((ENUM_GLOBAL_EFFECT_ARAD)sig->m_data[i].m_code,
                          sig->m_data[i].m_msg);
    }
