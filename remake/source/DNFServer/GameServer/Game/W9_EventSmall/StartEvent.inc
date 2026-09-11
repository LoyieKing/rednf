void CTrcOnOffEvent::StartEvent()            // ORIG 0x08285aae
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x13, 0);
    trace("Trc On Off Event Start!");
}