void CRelayBattleMgr::OnDiePlayer(int victimSeat, int killerSeat, bool bTime)
{
    if (victimSeat >= 0 && victimSeat < 8)
    {
        m_battles[victimSeat].m_state = 3;
    }
    if (killerSeat >= 0 && killerSeat < 8)
    {
        ++m_battles[killerSeat].m_killCount;
    }
    if (bTime && m_state == 1)
    {
        m_state = 2;
        m_lastFightStartTime = GlobalData::s_systemTime_.getCurSec();
        G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)1,
                                                m_pRoom->get_index(),
                                                (TIMER_MESSAGE)0x63, 3000,
                                                m_lastFightStartTime, 0);
    }
}