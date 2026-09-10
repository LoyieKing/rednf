void CMaxFatigueFactorEvent::EndEvent()
{
    SetEventFlag(false);
    m_maxFatigueFactor = 100;
    cMyTrace tr(__FUNCTION__, 0x2a, 0);
    tr("Max Fatigue Factor Event End!");
    CLocalChina_User::SendFatigueEvent();
