int WongWork::CHackAnalyzer::getServerHackAccumulatedCnt(
    WongWork::ENUM_HACKTYPE type) const
{
    return m_serverHackCntB[(int)type - 0x65];
