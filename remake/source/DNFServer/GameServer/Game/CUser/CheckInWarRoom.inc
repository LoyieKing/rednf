bool CUser::CheckInWarRoom()
{
    return (~(unsigned short)m_warRoomIndex & 0x8000) != 0;
}