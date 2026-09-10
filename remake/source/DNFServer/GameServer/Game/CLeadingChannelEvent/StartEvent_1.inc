void CLeadingChannelEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_channelFactor = param.m_duration;
    cMyTrace tr(__FUNCTION__, 0x1b, 0);
    tr("Leading Channel Event Start!");
}
