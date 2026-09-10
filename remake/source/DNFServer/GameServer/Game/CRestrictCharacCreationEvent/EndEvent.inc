void CRestrictCharacCreationEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x2b, 0);
    tr("Restrict Character Creation End!");
