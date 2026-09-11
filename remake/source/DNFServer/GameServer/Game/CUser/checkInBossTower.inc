bool CUser::checkInBossTower()
{
    return (~(unsigned short)m_bossTowerIdx & 0x8000) != 0;
}