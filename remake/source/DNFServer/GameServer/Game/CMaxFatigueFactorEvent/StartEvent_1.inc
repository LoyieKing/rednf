void CMaxFatigueFactorEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_maxFatigueFactor = param.m_duration;
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("Max Fatigue Factor Event Start!");
    CLocalChina_User::SendFatigueEvent();
