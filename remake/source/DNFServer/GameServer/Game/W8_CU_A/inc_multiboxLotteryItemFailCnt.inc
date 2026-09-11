// ===========================================================================
// 3b) CUser::inc_multiboxLotteryItemFailCnt() @0x080fdf56
// ===========================================================================
int CUser::inc_multiboxLotteryItemFailCnt()
{
    m_multiboxLotteryFailCnt += 1;
    return m_multiboxLotteryFailCnt;
}