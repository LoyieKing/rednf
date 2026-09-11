void CUser::SetBingoData(SIG_LOGIN_DATA& sig)
{
    // SetBingoData.md @0x0869126e：clear(BingoData@+0x8ebdc)、
    // m_eventId←sig.m_bingoEventId(+0x413c)、m_isWin←sig.m_bingoWinFlag(+0x4138)!=0。
    // 注：BingoData::clear 当前 owner（CUser.cpp）为空桩占位，语义修复归其域批次。
    m_bingo.clear();
    m_bingo.m_eventId = sig.m_bingoEventId;
    m_bingo.m_isWin = (sig.m_bingoWinFlag != 0);
}