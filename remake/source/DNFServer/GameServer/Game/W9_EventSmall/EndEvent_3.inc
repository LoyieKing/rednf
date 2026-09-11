void CAutoMarketConditionsControlEvent::EndEvent()    // ORIG 0x08109cf8
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x1f, 0);
    trace("CAutoMarketConditionsControlEvent End!");
}