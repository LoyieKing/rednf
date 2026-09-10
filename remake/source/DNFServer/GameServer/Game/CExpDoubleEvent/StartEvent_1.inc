void CExpDoubleEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_factor = (unsigned int)param.m_duration;
    cMyTrace tr(__FUNCTION__, 0x1a, 0);
    tr("Exp Double Event Start!");
