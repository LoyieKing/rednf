void BlueMarble::userThrowDice(CUser* user)
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x446,
                                  "BlueMarble::userThrowDice slot(%d)", seat);
        }
        else if (checkThrowDice(seat))
        {
            resetIndexInfo(seat);
            throwDice(seat);
            setMovePosition(seat, getDiceNumber());
            intoBoardZone(seat);
            sendBoardZoneActionInfo(seat, getDungeonIndex(seat),
                                    (unsigned int)getItemIndex(seat));
        }
    }
done:
    ;
