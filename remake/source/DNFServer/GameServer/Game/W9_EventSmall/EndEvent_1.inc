void CFatigueBuffEvent::EndEvent()           // ORIG 0x0811bcc0
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x1b, 0);
    trace("CFatigueBuffEvent Event End!");
