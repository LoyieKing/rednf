void CRestrictCharacCreationEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x17, 0);
    tr("Restrict Character Creation Start!");
