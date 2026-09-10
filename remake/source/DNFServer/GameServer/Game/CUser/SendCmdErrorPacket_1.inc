bool CUser::SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error, PAY_TYPE pay)
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(error);
    ((InterfacePacketBuf*)&packet)->put_byte((int)pay);   // ORIG 写 1 字节（put_int 会破坏包格式）
    ((InterfacePacketBuf*)&packet)->finalize(true);
    // ORIG 0x867c0a0-0x867c0c8：finalize 后 cMyTraceNoop()("CmdID %d, nErrorCode %d", cmd, error)
    // （format @0x8cefc8e；禁追踪 noop 空对象，临时 @-0x9）。1st=CmdID(cmd) 2nd=nErrorCode(error)。
    cMyTraceNoop()("CmdID %d, nErrorCode %d", (int)cmd, (int)error);
    return Send(packet);
