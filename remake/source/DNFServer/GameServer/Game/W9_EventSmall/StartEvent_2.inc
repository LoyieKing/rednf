void CFatigueBuffEvent::StartEvent()         // ORIG 0x0811bc54
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x11, 0);
    trace("CFatigueBuffEvent Event Start!");
