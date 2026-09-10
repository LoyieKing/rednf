void BlueMarble::setUserState(int seat, BlueMarbleUserState::T state)
{
    m_users[seat].setUserState(state);
