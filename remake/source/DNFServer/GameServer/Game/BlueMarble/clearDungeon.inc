void BlueMarble::clearDungeon(CUser* user)
{
    if (user == 0) goto done;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x48a,
                                  "BlueMarble::clearDungeon slot(%d)", seat);
        }
        else if (checkLastZoneDungeonClear(seat))
        {
            finishBlueMarble(seat);
            resultReward();
        }
    }
done:
    ;
