bool CUser::checkInDeathTower()
{
    return (~(unsigned short)m_deathTowerIndex & 0x8000) != 0;
}