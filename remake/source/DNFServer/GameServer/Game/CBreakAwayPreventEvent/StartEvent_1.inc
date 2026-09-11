// ORIG 0x08109e78：SetEventFlag(true) + makeRequest(true) +
//   cMyTrace(__FUNCTION__, 0x1b, 0)("CBreakAwayPreventEvent Start!")。
void CBreakAwayPreventEvent::StartEvent(Word_Param /*param*/)
{
    SetEventFlag(true);
    DB_LoadBreakAwayPreventData::makeRequest(true);
    cMyTrace trace(__FUNCTION__, 0x1b, 0);
    trace("CBreakAwayPreventEvent Start!");
}