void CCollectArchieveEventLog::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x23, 0);
    tr("Collect Archieve Event Log End!");
