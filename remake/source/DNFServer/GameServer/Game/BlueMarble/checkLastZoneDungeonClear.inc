bool BlueMarble::checkLastZoneDungeonClear(int seat)
{
    if (getState() == 2)
    {
        if (m_users[seat].getBoardZone() == getMaxBoardSize())
        {
            return 1;
        }
    }
    return 0;
}