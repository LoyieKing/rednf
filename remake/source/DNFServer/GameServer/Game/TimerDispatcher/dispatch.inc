int TimerDispatcher::dispatch(TimerEntry& entry)
{
    // [b28c-L18] N7 判词：ORIG `mov (%eax),%ebx` vptr 跨三条 entry 成员装载保活、
    // `call *%ebx`；4.4 恒间接 call 目标=%eax（成员装载改用 edx/ebx/ecx 轮换）。
    // 34=34 助记符序列全等、esp 槽值集全等，仅寄存器分配差，封顶。
    TimerSig* d = get_dispatcher(entry.msg);
    if (d == 0)
    {
        return 0;
    }
    return d->dispatch_sig(entry.param1, entry.param2, entry.param3);
