// ORIG 0x08109ed6：SetEventFlag(false) + makeRequest(false) +
//   cMyTrace(__FUNCTION__, 0x23, 0)("CBreakAwayPreventEvent End!")。
void CBreakAwayPreventEvent::EndEvent()
{
    SetEventFlag(false);
    DB_LoadBreakAwayPreventData::makeRequest(false);
    cMyTrace trace(__FUNCTION__, 0x23, 0);
    trace("CBreakAwayPreventEvent End!");
}