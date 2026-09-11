// ---------------------------------------------------------------------------
// ORIG 0x0811fe92：SetEventFlag(false) + m_channelFactor = 100 +
// cMyTrace(__FUNCTION__,0x23,0)("Leading Channel Event End!")。
void CLeadingChannelEvent::EndEvent()
{
    SetEventFlag(false);
    m_channelFactor = 100;
    cMyTrace tr(__FUNCTION__, 0x23, 0);
    tr("Leading Channel Event End!");
}
