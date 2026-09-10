bool CDisconnectDetecter::UnregisterUser(CUser* user)
{
    DetectedObjectKey key(user->GetUID(), user->get_unique_id() & 0xffff);
    std::map<DetectedObjectKey, int>::iterator it = m_map.find(key);
    if (it != m_map.end())
    {
        m_map.erase(it);
        return true;
    }
    return false;
