void CPCRoomBurningEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_factor = (int)(unsigned int)param.m_duration;
    cMyTrace tr(__FUNCTION__, 0x1c, 0);
    tr("PC Room Burning Event Start!");
