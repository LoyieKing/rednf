// ---------------------------------------------------------------------------
// ORIG 0x08116bb2：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x19,0)
// ("Collect Archieve Event Log Start!")。
void CCollectArchieveEventLog::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x19, 0);
    tr("Collect Archieve Event Log Start!");
}