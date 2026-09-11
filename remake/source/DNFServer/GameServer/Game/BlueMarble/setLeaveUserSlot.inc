void BlueMarble::setLeaveUserSlot(int seat)
{
    if (m_users[seat].getUser() == 0)
    {
        m_users[seat].resetBlueMarbleUserInfo();
        LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x2c4,
                              "BlueMarble::setLeaveUserSlot slot(%d)", seat);
    }
    else
    {
        m_users[seat].getUser()->setBlueMarbleIndex(-1);
        m_users[seat].getUser()->setMoveSpace((UserSpace::T)0);
        m_users[seat].getUser()->resetBlueMarbleUserDeadCount();
        m_users[seat].resetBlueMarbleUserInfo();
    }
}