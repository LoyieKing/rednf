bool TimerPvPDieUserRevive7::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    PvPDieUserRevive(p1, p2, p3, 7);
#if !remake_verify
    return 1;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}