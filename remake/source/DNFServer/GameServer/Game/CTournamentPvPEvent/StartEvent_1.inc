// ---------------------------------------------------------------------------
// ORIG 0x0828463c：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x18,0)
// ("Tournament PvP Channel Open Start")。
void CTournamentPvPEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("Tournament PvP Channel Open Start");
}