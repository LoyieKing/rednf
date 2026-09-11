bool TimerGlobalEffectItemBegin::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x08637e56：实参为 0x10(%ebp)=p2（先前误用 p3，已修正）。
    GlobalData::s_globalEffectManager_->beginReservedEffect(p2);
    return 1;
}