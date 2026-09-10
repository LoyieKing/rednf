void CLeadingChannelEvent::StartEvent()
{
    SetEventFlag(true);
    m_channelFactor = 100;
    cMyTrace tr(__FUNCTION__, 0x13, 0);
    tr("Leading Channel Event Start!");
