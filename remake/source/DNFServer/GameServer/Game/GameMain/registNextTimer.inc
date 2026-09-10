void TimerDeathTowerTopRankerReload::registNextTimer(long ms)
{
    // ORIG 0x8637854：
    //   ms==0 → 首次注册：now + rand(0..299)（秒）；
    //   否则 localtime 后强制 tm_hour=4、tm_min=30+rand(0..299)，mktime 归一
    //   （每日 04:30~08:29 随机重载点）；最终以 max(delay,1) 秒注册 msg=0x73。
    if (ms == 0)
        ms = static_cast<long>(s_systemTime_.getCurSec()) + get_rand_int(0x12c);
    else
    {
        std::tm t;
        localtime_r(&ms, &t);
        t.tm_hour = 4;
        t.tm_min = get_rand_int(0x12c) + 0x1e;
        t.tm_sec = 0;   // ORIG 0x86378af：movl $0x0,-0x3c(%ebp)（真语义：清残留秒）
        ms = mktime(&t);
    }
    int delay = static_cast<int>(ms) - s_systemTime_.getCurSec();
    // ORIG 0x86378f5：std::max 结果直接入 ebx 实参（mov (%eax),%ebx），无回写 delay 槽
    //（P24 内联实参形）。b28c-33：ORIG 实参槽序为 (%esp)=-0xc(常量 1)、0x4(%esp)=-0x10
    //（delay）⇒ 原码写 std::max(1, delay)（交换律等价，仅装载序差异）。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)6, 0,
                                (TIMER_MESSAGE)0x73, std::max(1, delay), 0, 0);
