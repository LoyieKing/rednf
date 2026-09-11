void BlueMarble::leaveUser(CUser* user)
{
    if (user == 0)
    {
        LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x3f9,
                              "BlueMarble::leaveUser user is NULL");
    }
    else
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x400,
                                  "BlueMarble::leaveUser slot(%d)", seat);
        }
        else
        {
            G_GameWorld()->outFromBlueMarble(user);
            setLeaveUserSlot(seat);
            setLeaveUserBlueMarbleState();
            sendBlueMarbleRoomInfo();
        }
    }
}