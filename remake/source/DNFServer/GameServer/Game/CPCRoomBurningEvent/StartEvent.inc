void CPCRoomBurningEvent::StartEvent()
{
    SetEventFlag(true);
    m_factor = 100;
    cMyTrace tr(__FUNCTION__, 0x14, 0);
    tr("PC Room Burning Event Start!");
