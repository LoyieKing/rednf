void CPvPExpPenaltyEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("CPvPExpPenaltyEvent Start!");
