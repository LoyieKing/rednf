// ---------------------------------------------------------------------------
// ORIG 0x08273cb8：SetEventFlag(true) +
// cMyTrace("virtual void CRestrictCharacCreationEvent::StartEvent()",0x17,0)
// ("Restrict Character Creation Start!")。
void CRestrictCharacCreationEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x17, 0);
    tr("Restrict Character Creation Start!");
}