unsigned int CUser::getPcRoomPlayTimeRewardCnt() const   // 0x08110dec：单 movzbl 0x8eba4(%eax)；b28d-Main 改 unsigned int（P248档①，见 CUser.h 注）
{
    return m_pcRoomPlayTimeRewardCnt;
}