void CUser::setPcRoomPlayTimeData(unsigned int startTime, unsigned int total,
                                  unsigned char rewardCnt)
{
    m_pcRoomPlayStartTime     = startTime;      // +0x8eb9c
    m_totalPcRoomPlayTime     = static_cast<int>(total);  // +0x8eba0(movl)
    m_pcRoomPlayTimeRewardCnt = rewardCnt;      // +0x8eba4
