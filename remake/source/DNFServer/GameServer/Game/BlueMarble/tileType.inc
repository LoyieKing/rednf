// b28c-1 AE: P0/N96 清绑 + P223 双岛（0x080d737c/0x080d737f）后 0 差。
int BlueMarble::tileType(int seat)
{
    int zone = m_users[seat].getBoardZone();
    // P0+范式22：ORIG 0x080d70f2 `test;sete %al` 物化 + `test al; jne` 出口
    //（body fall）。zone 槽 -0x10（0x080d70d2）。
    // b28c-1：P0/N96 清绑（P168a② 具名 bool + 正向 goto）。
    // ORIG 0x080d70f7：test al; jne 出口 —— 正向 goto（范式23）阻 4.4 xor 取反。
    if (m_pScript->getMap(getBlueMarbleMap()) == 0)
        goto T_OUT;
    {
        BlueMarbleTileScript* tile = m_pScript->getMap(getBlueMarbleMap())->getTile(
            (unsigned int)zone);
        // b28c-1：ORIG 双岛（0x080d737c mapNull 岛 / 0x080d737f tile==0 岛）
        //   ⇒ P223 多岛形：第二个条件也用 goto 到相邻标签。
        if (tile == 0)
            goto T_OUT2;
        switch (tile->m_type)   // ORIG 0x080d713b `mov (%eax),%eax`（int 读）
        {
            case 2:
            case 8:
                setDungeonIndex(seat,
                                getRandomDungeon(seat, (BlueMarbleTileScript::Type)2));
                break;
            case 3:
                setGoldBuff(seat, tile->m_value);
                break;
            case 4:
                setExpBuff(seat, tile->m_value);
                break;
            case 5:
                setDropBuff(seat, tile->m_value);
                break;
            case 6:
                setItemIndex(seat, getRandomItem());
                break;
            case 7:
                if (!getVisitZone(zone))
                {
                    setDungeonIndex(seat,
                                    getRandomDungeon(seat,
                                                     (BlueMarbleTileScript::Type)7));
                }
                break;
            case 9:
                setVisitZone(zone);
                setMovePosition(seat, tile->m_value);
                if (checkMoveFirstZone(seat, (BlueMarbleTileScript::Type)9))
                {
                    resetBuffInfo(seat);
                }
                intoBoardZone(seat);
                break;
            case 10:
                setVisitZone(zone);
                setMovePosition(seat, tile->m_value - getMovePosition(seat));
                if (checkMoveFirstZone(seat, (BlueMarbleTileScript::Type)10))
                {
                    resetBuffInfo(seat);
                }
                intoBoardZone(seat);
                break;
            case 11:
            case 12:
            case 13:
                setDungeonIndex(seat, tile->m_value);
                break;
        default:
            break;
        }
    }
T_OUT2:
    ;
T_OUT:
    ;
}