bool CDisconnectDetecter::RefreshRecvTime(CUser* user)
{
    DetectedObjectKey key(user->GetUID(), user->get_unique_id() & 0xffff);
    std::map<DetectedObjectKey, int>::iterator it = m_map.find(key);
    if (it != m_map.end())
    {
        it->second = GlobalData::s_systemTime_.getCurSec();
    }
    else
    {
        return false;
    }
    return true;
