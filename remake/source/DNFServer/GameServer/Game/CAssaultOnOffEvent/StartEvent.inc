void CAssaultOnOffEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__PRETTY_FUNCTION__, 0xe, 0);
    trace("Assault On Off Event Start!");
