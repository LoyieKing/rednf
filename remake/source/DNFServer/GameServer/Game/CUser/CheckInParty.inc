bool CUser::CheckInParty()
{
    return (~(unsigned short)m_partyIndex & 0x8000) != 0;
}