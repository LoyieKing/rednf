void CWeekendBonusEvent::StartEvent()        // ORIG 0x081175ba
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x12, 0);
    trace("CWeekendBonusEvent Event Start!");
