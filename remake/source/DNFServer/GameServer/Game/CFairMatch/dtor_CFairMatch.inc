CFairMatch::~CFairMatch()
{
    --nWaitingRoomListRefCount_;
    if (pWaitingRoomList_ != 0 && nWaitingRoomListRefCount_ == 0)
    {
        ::operator delete(pWaitingRoomList_);
    }
    m_fieldC0 = 0;
