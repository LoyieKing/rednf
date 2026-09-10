void CRelayBattleMgr::OnStart()
{
    m_lastFightStartTime = GlobalData::s_systemTime_.getCurSec();
    m_readyCount = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (m_battles[i].m_state == 1)
        {
            ++m_readyCount;
        }
    }
    G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1,
                                            m_pRoom->get_index(),
                                            (TIMER_MESSAGE)0x63, 7000,
                                            m_lastFightStartTime, 0);
