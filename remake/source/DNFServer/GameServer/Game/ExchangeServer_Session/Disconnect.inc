bool CSession::Disconnect()
{
    // [b28c-6] N7：ES() 虚调用槽寄存器 %ecx↔%eax 互换 + this 中转寄存器互换
    // （ORIG `mov (%eax),%ecx; mov <A>,%eax; mov this,%edx; mov %edx,0x4(%esp);
    // mov %eax,(%esp); call *%ecx`），4.4 恒目标 %eax，源码不可翻（ndiff 12）。
    cMyTrace(__PRETTY_FUNCTION__, 0x25e, 0)("Disconnect Exchange Server");

    if (CSession::exchange_server_ != 0)
    {
        // vtable+4 UnregisterExchangeServer(channelNo)
        ES()->UnregisterExchangeServer(GetChannelNo());
    }

    disconnect();

    // vtable+12 FreeSession(this)
    ES()->FreeSession(this);
    return true;   // ORIG 尾 `mov $0x1,%eax`
