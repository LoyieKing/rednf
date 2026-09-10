COnlinePreliminary::~COnlinePreliminary()
{
    nWaitingRoomListRefCount_ = nWaitingRoomListRefCount_ - 1;
    if (pWaitingRoomList_ != 0 && nWaitingRoomListRefCount_ == 0)
        delete pWaitingRoomList_;
