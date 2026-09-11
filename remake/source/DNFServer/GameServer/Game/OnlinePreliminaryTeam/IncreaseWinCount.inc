// ORIG T 0x085883fe：++win(@0xc)；m_hasWinFlag(+0x3c)=1。
void COnlinePreliminaryTeam::IncreaseWinCount()
{
    ++m_winCount;
    m_hasWinFlag = 1;
}