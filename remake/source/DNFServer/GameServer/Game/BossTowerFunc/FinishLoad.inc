bool Global::CBossTowerFunc::FinishLoad(CUser* user)  // ORIG T 0x08146916
{
    switch (user->get_state())
    {
    case 0xa:
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower != 0)
            {
                tower->handleFinishLoading(user);
            }
        }
        return 1;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleFinishLoading(user);  // ORIG 直调 0x08143f6a（非虚分派）
            }
        }
        return 1;
    }
    return 0;
