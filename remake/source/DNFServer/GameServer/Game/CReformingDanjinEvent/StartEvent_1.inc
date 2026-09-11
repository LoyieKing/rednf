// ---------------------------------------------------------------------------
// ORIG 0x0826e9e8：与 StartEvent() 同体（SetEventFlag(true) + trace，
// file 串为 "virtual void CReformingDanjinEvent::StartEvent(Word_Param)"，
// 行 0x19）。
void CReformingDanjinEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x19, 0);
    tr("Reforming Danjin Event Start!");
}