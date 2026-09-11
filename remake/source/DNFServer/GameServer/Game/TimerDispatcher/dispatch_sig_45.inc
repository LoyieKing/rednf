bool TimerEventChristmas::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 三方法为静态语义（调用点无 this 压栈，直接操作全局 0x943c138），
    // 静态成员直调（ORIG 8635710/863571b/8635726 无 temp 对象槽，帧 0x18）。
    CServerEvent::SubExpRate(p2);
    CServerEvent::TurnOffEvent(p3);
    // ORIG：结果不落实名槽，直接 movswl %ax,%edx 寄存器扩展后入参。
    Inter_MonitorServerEventStart::SendEventNotiPacket(false,
                                                        static_cast<short>(p3),
                                                        static_cast<short>(CServerEvent::GetEventInterval(p3)), 0);
    return 1;
}