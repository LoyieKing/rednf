bool TimerDeathTower::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    WongWork::CDeathTower* tower = G_CGameManager()->getDeathTower(p1);
    if (tower == NULL) {
        return 0;
    }
    tower->onTimer(static_cast<TIMER_MESSAGE>(p3), p2);
    return 1;
}