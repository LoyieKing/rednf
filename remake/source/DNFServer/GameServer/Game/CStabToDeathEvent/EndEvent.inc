// ---------------------------------------------------------------------------
// ORIG 0x0827f616：SetEventFlag(false) +
// cMyTrace(&_ZZN17CStabToDeathEvent8EndEventEvE12__FUNCTION__,0x1d,0)
// ("CStabToDeathEvent End!")。
void CStabToDeathEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1d, 0);
    tr("CStabToDeathEvent End!");
}