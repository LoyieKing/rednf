CFairMatch::CFairMatch()
{
    if (pWaitingRoomList_ == 0)
    {
        pWaitingRoomList_ = new CMultiMatchRoomList();
    }
    ++nWaitingRoomListRefCount_;
    m_fieldC0 = 0;
