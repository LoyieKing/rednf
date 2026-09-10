void CGmRegistEvent::StartEvent()
{
    DB_UpdateGMData::makeRequest(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x16, 0);
    tr("CGmRegistEvent Start!");
