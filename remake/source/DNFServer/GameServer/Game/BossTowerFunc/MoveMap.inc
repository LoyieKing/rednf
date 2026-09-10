bool Global::CBossTowerFunc::MoveMap(CUser* user, unsigned short* mapIndex)  // ORIG T 0x08146ba2
{
    switch (user->get_state())
    {
    case 0xa:
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower != 0)
            {
                tower->handleMoveMap();
            }
        }
        return 1;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleMoveMap(mapIndex);  // ORIG vptr+0x18 = 槽[6] handleMoveMap
            }
        }
        return 1;
    }
    return 0;
