void CGmRegistEvent::StartEvent(Word_Param param)
{
    DB_UpdateGMData::makeRequest(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1d, 0);
    tr("CGmRegistEvent Start!");
