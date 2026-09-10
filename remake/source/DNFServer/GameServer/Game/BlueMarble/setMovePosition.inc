int BlueMarble::setMovePosition(int seat, int pos)
{
    m_users[seat].setBoardZone(pos, getMaxBoardSize());
