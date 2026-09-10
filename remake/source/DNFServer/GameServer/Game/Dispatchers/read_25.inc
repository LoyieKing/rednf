int Dispatcher_GM_Command::read(PacketBuf& packet, MSG_BASE& msg)
{
    (void)packet; (void)msg;
    return 0;  // 同上：ORIG 无 read，逐字段在 dispatch_sig 内读取
