void CDeathTowerWinPointEvent::StartEvent(Word_Param param)
{
    m_ratio = (unsigned int)param.m_duration;
    StartEvent();
