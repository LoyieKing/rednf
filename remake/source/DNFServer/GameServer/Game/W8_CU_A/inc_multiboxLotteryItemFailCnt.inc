int CUser::inc_multiboxLotteryItemFailCnt()
{
    m_multiboxLotteryFailCnt += 1;
    return m_multiboxLotteryFailCnt;
