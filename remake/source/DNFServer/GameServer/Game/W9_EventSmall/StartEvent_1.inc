void CTrcOnOffEvent::StartEvent(Word_Param)  // ORIG 0x08285b00
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x19, 0);
    trace("Trc On Off Event Event Start!");
