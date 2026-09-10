void CTrcOnOffEvent::EndEvent()              // ORIG 0x08285b52
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x1f, 0);
    trace("Trc On Off Event Event End!");
