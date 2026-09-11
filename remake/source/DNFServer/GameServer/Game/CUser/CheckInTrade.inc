bool CUser::CheckInTrade()
{
    return (~(unsigned short)m_tradeSpaceIdx & 0x8000) != 0;
}