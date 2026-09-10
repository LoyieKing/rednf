int advancealtar::Timer_MainTick::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x812f798: G_CGameManager()->onTimeAdvanceAltar(); RegistNextTimerGage(); return 1
    G_CGameManager()->onTimeAdvanceAltar();
    RegistNextTimerGage();
    return 1;
