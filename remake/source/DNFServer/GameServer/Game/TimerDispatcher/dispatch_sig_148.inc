// [b28d-solo-TimerDispatcher] P214①/P225 正极性形：ORIG 是
// `bVar1 = (ev != 0 && IsEventing(0))` 物化岛（je L0 / test;je L0 / mov$1;jmp / L0:mov$0）
// ⇒ 源码必须写正极性 `if (ev != 0 && ev->IsEventing(0)) { body } return 1;`
// （旧码 `ev==0 || !IsEventing` 使 4.4 多 `xor $1` 且 body 块错位）。
// 配套：rgf 实参内联（ORIG time() 结果驻 %ebx 跨 getRewardStayTime），ORIG 0x0863b928。
// [b28d-solo-TimerDispatcher] 已 AE：P214①/P225 正极性 `if (ev != 0 && IsEventing(0))`
// + rgf 实参内联（time()→%ebx），53/53 全等。
bool Timer_StayTimeEvent::dispatch_sig(int /*p1*/, int /*p2*/, memberIdentificationNumber_t /*p3*/)
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
}