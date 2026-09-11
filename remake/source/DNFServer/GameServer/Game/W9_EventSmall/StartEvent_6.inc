void CAutoMarketConditionsControlEvent::StartEvent()  // ORIG 0x08109c7e
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x13, 0);
    trace("CAutoMarketConditionsControlEvent Start!");
}