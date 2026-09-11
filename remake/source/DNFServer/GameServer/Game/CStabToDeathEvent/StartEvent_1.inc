// ---------------------------------------------------------------------------
// ORIG 0x0827f5c4：与 StartEvent() 同体（SetEventFlag(true) + trace，
// file 串为 __FUNCTION__ 局部静态（E12__FUNCTION__），行 0x17）。
void CStabToDeathEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x17, 0);
    tr("CStabToDeathEvent Start!");
}