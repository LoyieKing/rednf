void CUser::setBreakAwayRewardData(bool flag)
{
    // ORIG 0x0831d50c：与 SetPuUser 同形 bool=bool 直落（成员已 bool）
    m_breakAwayRewardData = flag;
}