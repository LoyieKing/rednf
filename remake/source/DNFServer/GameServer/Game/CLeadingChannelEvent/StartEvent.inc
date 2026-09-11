// ---------------------------------------------------------------------------
// ORIG 0x0811fdd8：SetEventFlag(true) + m_channelFactor = 100 +
// cMyTrace(__FUNCTION__,0x13,0)("Leading Channel Event Start!")。
void CLeadingChannelEvent::StartEvent()
{
    SetEventFlag(true);
    m_channelFactor = 100;
    cMyTrace tr(__FUNCTION__, 0x13, 0);
    tr("Leading Channel Event Start!");
}
