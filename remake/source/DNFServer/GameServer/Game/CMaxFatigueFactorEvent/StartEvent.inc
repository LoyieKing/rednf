void CMaxFatigueFactorEvent::StartEvent()
{
    SetEventFlag(true);
    m_maxFatigueFactor = 100;
    cMyTrace tr(__FUNCTION__, 0x16, 0);
    tr("Max Fatigue Factor Event Start!");
