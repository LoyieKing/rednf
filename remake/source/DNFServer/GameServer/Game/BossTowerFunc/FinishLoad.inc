// ----------------------------------------------------------------------------
// ORIG T 0x08146916 _ZN6Global14CBossTowerFunc10FinishLoadEP5CUser
// state==10 → CDeathTower::handleFinishLoading；state==12 →
// CBossTower::handleFinishLoading（直调）；其它返回 0。前两分支返回 1。
// ----------------------------------------------------------------------------
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
}