// ---------------------------------------------------------------------------
// ORIG 0x08273d68：SetEventFlag(false) +
// cMyTrace("virtual void CRestrictCharacCreationEvent::EndEvent()",0x2b,0)
// ("Restrict Character Creation End!")。
void CRestrictCharacCreationEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x2b, 0);
    tr("Restrict Character Creation End!");
}