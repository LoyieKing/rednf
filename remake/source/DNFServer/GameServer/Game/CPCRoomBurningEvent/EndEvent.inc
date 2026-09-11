void CPCRoomBurningEvent::EndEvent()
{
    SetEventFlag(false);
    m_factor = 100;
    cMyTrace tr(__FUNCTION__, 0x24, 0);
    tr("PC Room Burning Event End!");
}