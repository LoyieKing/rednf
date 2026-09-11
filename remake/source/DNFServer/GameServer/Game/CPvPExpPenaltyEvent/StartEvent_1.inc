// ---------------------------------------------------------------------------
// ORIG 0x08268dd8：与 StartEvent() 同体（SetEventFlag(true) + trace，
// __FUNCTION__ 静态 0x08bf3f10 __FUNCTION__，行 0x18）。
void CPvPExpPenaltyEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("CPvPExpPenaltyEvent Start!");
}