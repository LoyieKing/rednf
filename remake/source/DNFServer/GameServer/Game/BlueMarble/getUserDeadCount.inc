int BlueMarble::getUserDeadCount(int seat) const
{
    return m_users[seat].getUser()->getBlueMarbleUserDeadCount();
