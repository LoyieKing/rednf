bool CUser::CheckInPvp()
{
    return (~(unsigned short)m_pvpRoomIdx & 0x8000) != 0;
}