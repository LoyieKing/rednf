bool TimerEventCoinRefill::dispatch_sig(int /*p1*/, int /*p2*/, memberIdentificationNumber_t /*p3*/)
{
    // ORIG 0x086364c4：GetRepeatEvent(0xd) → receiver->IsEventing(0)（vtbl+0x34）
    // → GameWorld::ResetAllCoins() → trace → registNextRefillTimer()。
    CCoinRefillEvent* event =
        static_cast<CCoinRefillEvent*>(GlobalData::s_event_manager->GetRepeatEvent(0xd));
    if (event->IsEventing(0))   // ORIG 0x086364c4：无 event!=0 检查，直接解引用
    {
        G_GameWorld()->ResetAllCoins();
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xadc, 0);
        trace("Coin refilled by event!");
        event->registNextRefillTimer();
    }
    return 1;
}