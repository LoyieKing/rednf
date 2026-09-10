bool CSession::OnConnect()
{
    // vtable+0x10（slot5）GetServerType()；sete 比较返回值==1(EXCHANGE_SERVER_SLAVE)。
    // [b28c-6][N7] ES() 虚调用槽寄存器 %eax↔%edx 互换（4.4 恒形），源码不可翻（ndiff 8）。
    if ((int)ES()->GetServerType() == 1)
    {
        PacketGuard guard;
        guard.clear();
        guard.put_header(1, 0x82);
        guard.put_int(GetServerChannelNo());
        guard.put_int((int)inet_addr(GetServerIP()));
        guard.put_short(GetServerPORT());
        guard.finalize(false);          // ORIG 0x8490681：Send 前 finalize，否则 Send 判定未封包不发
        // ORIG 0x8490698：mov %eax,%ebx 跨 dtor 存活后 mov %ebx,%eax 透传（bool→bool，
        // 无 test/setne 规范化——Send 真实返回 bool，PRETTY_FUNCTION 串实证）。
        return Send(guard);
    }
    return true;                        // ORIG 0x84906c2：mov $1,%ebx
