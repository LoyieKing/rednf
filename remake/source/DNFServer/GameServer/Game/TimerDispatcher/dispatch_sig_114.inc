int TimerItemUnlockWaiting::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);
    if (user == NULL) {
        return 0;
    }
    if (user->getCurCharacR() == NULL) {
        return 0;
    }
    if ((unsigned int)user->getCurCharacNo() != p3) {
        return 0;
    }
    user->OnItemUnlockWaitTimeout(p2, p3);
    return 1;
