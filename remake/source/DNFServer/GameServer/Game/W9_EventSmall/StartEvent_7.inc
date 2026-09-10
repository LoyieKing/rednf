void CAutoMarketConditionsControlEvent::StartEvent(Word_Param)  // ORIG 0x08109cd0
{
    SetEventFlag(true);
    DB_LoadAutoMarketConditionsControl::makeRequest(true);
