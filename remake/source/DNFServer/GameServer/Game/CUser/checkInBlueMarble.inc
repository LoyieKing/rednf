bool CUser::checkInBlueMarble()
{
    return (~(unsigned short)m_blueMarbleIndex & 0x8000) != 0;
}