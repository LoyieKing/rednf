int DisPatcher_Login::read(PacketBuf& packet, MSG_BASE& msg)

{
    // ORIG 0x08232a6a：空实现，恒返 0（mov $0x0,%eax; pop %ebp; ret）。
    return 0;
}
