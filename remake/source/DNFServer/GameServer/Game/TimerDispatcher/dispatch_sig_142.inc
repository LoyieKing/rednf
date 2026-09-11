bool Timer_TowerOnTime::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    WongWork::CBossTower* tower = G_CGameManager()->getBossTower(p1);
    if (tower == NULL) {
        return 0;
    }
    tower->_onTimerProcess(static_cast<TIMER_MESSAGE>(p3), p2);
    return 1;
}