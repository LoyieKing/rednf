// ---------------------------------------------------------------------------
// ORIG 0x0827f572：SetEventFlag(true) +
// cMyTrace(&_ZZN17CStabToDeathEvent10StartEventEvE12__FUNCTION__,0x11,0)
// ("CStabToDeathEvent Start!")。
void CStabToDeathEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x11, 0);
    tr("CStabToDeathEvent Start!");
}