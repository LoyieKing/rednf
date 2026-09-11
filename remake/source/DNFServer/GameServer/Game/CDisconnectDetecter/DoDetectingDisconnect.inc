bool CDisconnectDetecter::DoDetectingDisconnect()
{
    for (std::map<DetectedObjectKey, int>::iterator it = m_map.begin();
         it != m_map.end(); )
    {
        if (_IsTimeout(it->second))
        {
            CUser* user = _GetUser(it->first.m_uid, it->first.m_uniqueId);
            if (user != 0)
            {
                OnDisconnect(user);
            }
            m_map.erase(it++);
        }
        else
        {
            ++it;
        }
    }
    return !m_map.empty();
}