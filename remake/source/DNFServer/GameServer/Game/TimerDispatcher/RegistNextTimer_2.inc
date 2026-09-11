// ORIG T 0x8635bdc（RegistNextTimer.md 全文转写；声明已在 TimerDispatcher.h，
// 签名 static void；ORIG 恒返回 eax=1，void 化由现有头口径决定）
// ORIG T 0x08635bdc：InsertTimer(2,0,0x59,0x708,0,0)；返回 1。
int TimerQueryCounter::RegistNextTimer()
{
    // ORIG：delay 先物化到局部槽 -0xc(%ebp)（movl $0x708,-0xc），再两步入栈。
    int delay = 0x708;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x59,
                                delay, 0, 0);
    return 1;
}