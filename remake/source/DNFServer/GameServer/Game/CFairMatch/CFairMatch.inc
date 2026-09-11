// ORIG C1/C2 0x856284a：IMatch 基类、PVP_ROOM_INFO@+8 与 TeamInfoThird[2]@+0x1c
// 子对象由默认初始化完成（vptr 同步）；随后延迟单例化 CMultiMatchRoomList 并
// 引用计数++；最后 m_fieldC0=0。
CFairMatch::CFairMatch()
{
    if (pWaitingRoomList_ == 0)
    {
        pWaitingRoomList_ = new CMultiMatchRoomList();
    }
    ++nWaitingRoomListRefCount_;
    m_fieldC0 = 0;
}