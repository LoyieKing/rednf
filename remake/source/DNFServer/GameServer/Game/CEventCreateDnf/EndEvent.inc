void CEventCreateDnf::EndEvent()
{
    cMyTrace trace("CEventCreateDnf::EndEvent()", 0x38, 0);
    trace("[Taiwan, CreateDnfEvent] End Event.");
    m_startHour = 0;
    m_endHour = 0;
    m_reloadInterval = 0;
    setState(State::kWaiting);
    SetEventFlag(false);
    // ORIG 0x08163cd0：不清 +0x16 m_rate。
