void CLeadingChannelEvent::EndEvent()
{
    SetEventFlag(false);
    m_channelFactor = 100;
    cMyTrace tr(__FUNCTION__, 0x23, 0);
    tr("Leading Channel Event End!");
}
