int Timer_StayTimeEvent::dispatch_sig(int /*p1*/, int /*p2*/, unsigned int /*p3*/)
{
    CEventStayTime* ev =
        (CEventStayTime*)GlobalData::s_event_manager->GetRepeatEvent(0x9f);
    if (ev != 0 && ev->IsEventing(0))
    {
        // ORIG +0x5a/+0x67/+0x7a：右起求值 time() → getRewardStayTime()，
        // ctor 形参序 (rewardStayTime, now)（ORIG +0x6c/%ebx、+0x70/%eax 压栈实证）。
        CEventStayTime::RewardGoGoFighter visitor(ev->getRewardStayTime(),
                                                  time((time_t*)0));
        // ORIG +0x97: GameWorld::ProcessAllUsers<CEventStayTime::RewardGoGoFighter>(visitor)
        G_GameWorld()->ProcessAllUsers(visitor);
        // ORIG +0xa2: CEventStayTime::RegistNextTimer()
        ev->RegistNextTimer();
    }
    return 1;
