void CReformingDanjinEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1f, 0);
    tr("Reforming Danjin Event End!");
