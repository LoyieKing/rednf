int DisPatcher_CheckConn::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x08232a7e：空实现，恒返 0（mov $0x0,%eax; pop %ebp; ret）。
    return 0;
