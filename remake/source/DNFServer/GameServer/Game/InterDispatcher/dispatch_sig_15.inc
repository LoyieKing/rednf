int Inter_DeleteChannel::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    const Inter_DeleteChannelPktView* pPkt = reinterpret_cast<const Inter_DeleteChannelPktView*>(data);
    // ORIG +0x0b：单级带偏移读（无中间 int* 槽）。
    if (pPkt->m_8 == 1)
    {
        return 0x806;
    }
    return 0;
