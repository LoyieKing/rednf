// ---------------------------------------------------------------------------
// ORIG 0x0811d4f4：与 StartEvent() 同体（file 串 "StartEvent"，行 0x18；
// 忽略 param）。
void CIntegratedPvPServerMatchEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("CIntegratedPvPServerMatchEvent Start!");
}