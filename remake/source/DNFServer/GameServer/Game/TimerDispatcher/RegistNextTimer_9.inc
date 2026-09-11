// ORIG T 0x08636f26 (_ZN26TimerCheckForcedDisconnect15RegistNextTimerEP5CUser)
// 头声明为 static bool；ORIG eax 恒为 1，语义等价。
// TimerDispatcher.h 现签名 `static bool RegistNextTimer(CUser*)` 即可，无需改动。
bool TimerCheckForcedDisconnect::RegistNextTimer(CUser* user)
{
    // ORIG 0x08636f26：delay 先落 -0xc 槽（movl $0xa,-0xc）；两次 call 结果
    // 直落 %esi/%ebx（register 形态，探针 x10 r1）。
    int delay = 10;
    register int uid16 = static_cast<unsigned short>(user->get_unique_id());   // ORIG movzwl %ax,%esi 定义处零扩展
    register unsigned int uid = user->GetUID();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, uid,
                                (TIMER_MESSAGE)0x6a, delay, uid16, 0);
    return 1;
}