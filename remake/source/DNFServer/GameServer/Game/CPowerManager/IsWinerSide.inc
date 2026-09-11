bool CPowerManager::IsWinerSide(char side)
{
    if (m_winnerSide == 0 || side == 0)
        return false;
    return m_winnerSide == side;
}