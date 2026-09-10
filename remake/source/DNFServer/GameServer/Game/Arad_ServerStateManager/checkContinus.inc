void Arad_ServerStateManager::checkContinus()
{
    Guard<Mutex> guard(&m_lock);
    const unsigned long curSec = GlobalData::s_systemTime_.getCurSec();

    std::map<ENUM_GLOBAL_EFFECT_ARAD, AradServerStateMessage>::iterator it =
        m_stateMap.begin();
    while (it != m_stateMap.end())
    {
        if (curSec >= it->second.m_endSec && it->second.m_active == 1)
        {
            it->second.m_active = 0;
            sendEffectPacket(it->first, it->second, 0);
            saveServerState(K_NOTIFY_EVENT_CATEGORY_SERVER_STAT, it->first,
                            it->second, false);
            cMyTrace(__PRETTY_FUNCTION__, 0x56, 0)(
                "[SERVER_STAT] delete server stat. (category:%d, code:%d, end:%u)",
                1, (int)it->first, it->second.m_endSec);
            m_stateMap.erase(it++);
        }
        else
        {
            ++it;
        }
    }
    attachTimer();
