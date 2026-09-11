int Dispatcher_LogIngameAdvertisement::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e4df2：check_error 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x5d83)；err==0 → 广告管理器写日志。
    // [G6-243] P-L11/P183：ORIG 无 err 具名槽（check_error 返回值直写 param->m_4，
    //   0x081e4e1d `mov %eax,0x4(%edx)`），后续两次检查从内存重读（0x081e4e23/0x081e4e37）；
    //   param 走具名视图指针槽 -0x20（0x081e4dfb `mov 0x14(%ebp),%eax; mov %eax,-0x20(%ebp)`）。
    Err4ParamView* pParam = reinterpret_cast<Err4ParamView*>(&param);
    pParam->m_4 = check_error(user, msg);
    if (pParam->m_4 > 0)
    {
        return 0;
    }
    if (pParam->m_4 < 0)
    {
        // ORIG 0x081e4e5f `call LineFunc` → 0x081e4e64 `jmp 081e4ebf`（函数尾），
        // 无 `mov $0x0,%eax` ⇒ 返回值为 LineFunc 结果直通（err>0 臂才是 `mov $0x0,%eax`）。
        return LineFunc(0x5d83,
                        __PRETTY_FUNCTION__,
                        pParam->m_4, 0);
    }

    // ORIG 字段读取顺序：+0x11 uint → +0x10 char(movzbl 0x10(%eax),%eax; movsbl %al,%edi)
    // → +0xe short(movzwl 0xe(%eax),%eax; movswl %ax,%esi) → +0xd char(movsbl %al,%ebx)。
    // [G6-243] P183：三字段无字节具名槽（旧译 flag/val/type 各占一槽），直接内联进实参
    //   （右→左求值：+0x10→%edi、+0xe→%esi、+0xd→%ebx）；this 内联在调用点
    //   （ORIG `call InGameADManagerInstance` 在字段读取之后、填实参之前）。
    LogIngameAdView& vmsg = reinterpret_cast<LogIngameAdView&>(msg);
    // [G6-243] P-L11/P261：+0x11 无具名槽（ORIG 0x081e4e6c 首算 → 唯一 `-0x2c` 槽 =
    //   跨 `call InGameADManagerInstance` 的保活临时），内联第 5 实参。
    InGameADManagerInstance()->WriteLog(user, (char)vmsg.m_c0d,
                                        (short)vmsg.m_s0e,
                                        (char)vmsg.m_c10, vmsg.m_u11);
    return 0;
}