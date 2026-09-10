char Global::CBossTowerFunc::DieCharacInDungeon(CUser* user)
{
    switch (user->get_state())
    {
    case 0xa:
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower != 0)
            {
                tower->handleDieUser(user);
            }
        }
        return 1;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleDieUser(user);  // ORIG vptr+0x10 = 槽[4] handleDieUser
            }
        }
        return 1;
    }
    return 0;
