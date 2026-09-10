void CPCRoomWorldDropEvent2nd::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1b, 0);
    tr("PCRoom World Drop Event 2nd End!");
