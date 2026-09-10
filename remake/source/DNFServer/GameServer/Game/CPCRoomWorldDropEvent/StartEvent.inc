void CPCRoomWorldDropEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x21, 0);
    tr("PCRoom World Drop Event Start!");
