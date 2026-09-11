// ---------------------------------------------------------------------------
// ORIG 0x0811cce2：DB_UpdateGMData::makeRequest(true) +
// cMyTrace(__PRETTY_FUNCTION__,0x16,0)
// ("CGmRegistEvent Start!")（无 SetEventFlag）。
void CGmRegistEvent::StartEvent()
{
    DB_UpdateGMData::makeRequest(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x16, 0);
    tr("CGmRegistEvent Start!");
}