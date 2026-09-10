void BlueMarble::setUserDeadCount(int count)
{
    m_users[count].getUser()->increaseBlueMarbleUserDeadCount();
