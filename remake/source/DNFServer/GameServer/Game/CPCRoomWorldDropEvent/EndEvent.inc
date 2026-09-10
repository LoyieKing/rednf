void CPCRoomWorldDropEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x2c, 0);
    tr("PCRoom World Drop Event End!");
