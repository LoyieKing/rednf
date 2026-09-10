void BlueMarble::enterDungeon(CUser* user)
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x45c,
                                  "BlueMarble::enterDungeon slot(%d)", seat);
        }
        else
        {
            setUserState(seat, (BlueMarbleUserState::T)2);
        }
    }
done:
    ;
