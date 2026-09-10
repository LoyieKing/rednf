void BlueMarble::giveupDungeon(CUser* user)
{
    // b28c-1：P223 多岛形（ORIG 三岛 0x080d96e7/ea/ed：user==0、mapNull、tile==0
    //  各 goto 到相邻尾标签），P0/N96 清绑后以裸条件 + goto 复现。
    if (user == 0) goto DONE1;
    {
        int seat = getUserSlot(user);
        if (seat == -1)
        {
            LogManager::logFormat(1, "BlueMarble.cpp", __PRETTY_FUNCTION__, 0x46b,
                                  "BlueMarble::giveupDungeon slot(%d)", seat);
        }
        else
        {
            if (m_pScript->getMap(getBlueMarbleMap()) == 0)
                goto DONE2;
            {
                BlueMarbleTileScript* tile = m_pScript->getMap(getBlueMarbleMap())->getTile(
                    (unsigned int)m_users[seat].getBoardZone());
                if (tile == 0)
                    goto DONE3;
                {
                    if (checkMoveFirstZone(seat,
                                           (BlueMarbleTileScript::Type)tile->m_type))
                    {
                        setMovePosition(seat, 1 - getMovePosition(seat));
                        resetBuffInfo(seat);
                        sendTileGiveUpInfo(seat);
                    }
                    setUserState(seat, (BlueMarbleUserState::T)1);
                }
            }
        }
    }
DONE1:
    ;
DONE2:
    ;
DONE3:
    ;
