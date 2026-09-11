int BlueMarble::getMovePosition(int seat) const
{
    return m_users[seat].getBoardZone();
}