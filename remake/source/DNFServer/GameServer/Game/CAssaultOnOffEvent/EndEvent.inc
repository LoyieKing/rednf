void CAssaultOnOffEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x18, 0);
    trace("Assault On Off Event End!");
