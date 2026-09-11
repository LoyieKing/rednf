// ORIG D1/D2 0x856292e / D0 0x85629ca。
// 体：引用计数--并在归零且单例仍存活时 delete；m_fieldC0=0。
// （ORIG 尾部 operator delete(this) 仅出现在 D0，由编译器按虚析构语义生成。）
CFairMatch::~CFairMatch()
{
    --nWaitingRoomListRefCount_;
    if (pWaitingRoomList_ != 0 && nWaitingRoomListRefCount_ == 0)
    {
        ::operator delete(pWaitingRoomList_);
    }
    m_fieldC0 = 0;
}