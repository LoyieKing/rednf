void CEventStayTime::StartEvent()
{
    SetEventFlag(true);
    cMyTrace(__PRETTY_FUNCTION__, 10, 0)(
        "[Taiwan, StayEvent] Start! StayTime:%dMin, CheckTime:%dMin",
        m_rewardStayTime, m_intervalStayTime);
    RegistNextTimer();
