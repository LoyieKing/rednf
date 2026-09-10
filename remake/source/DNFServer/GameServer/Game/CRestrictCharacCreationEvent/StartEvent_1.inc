void CRestrictCharacCreationEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_limitType = param.m_duration;
    cMyTrace tr(__FUNCTION__, 0x23, 0);
    tr("Restrict Character Creation Start!");
