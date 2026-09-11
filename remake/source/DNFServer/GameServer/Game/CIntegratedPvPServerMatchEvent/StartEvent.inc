// ---------------------------------------------------------------------------
// ORIG 0x0811d4a2：SetEventFlag(true) +
// cMyTrace(__FUNCTION__,0x12,0)("CIntegratedPvPServerMatchEvent Start!")。
void CIntegratedPvPServerMatchEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x12, 0);
    tr("CIntegratedPvPServerMatchEvent Start!");
}