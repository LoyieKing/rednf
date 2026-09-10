bool CDisconnectDetecter::RegisterUser(CUser* user)
{
    int mapSize = m_map.size();
    long curSec = GlobalData::s_systemTime_.getCurSec();
    DetectedObjectKey key(user->GetUID(), user->get_unique_id() & 0xffff);
    std::pair<std::map<DetectedObjectKey, int>::iterator, bool> ret =
        m_map.insert(std::make_pair<DetectedObjectKey, long>(key, curSec));
    if (ret.second == false)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x25, 0);
        trace("RegisterUser() wrong");
        return 0;
    }
    if (mapSize == 0)
    {
        TimerDetectDisconnect::registNextTimer();
    }
    return 1;
