int DisPatcher_CheckSecurityProtection::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x08264810：恒定返回 0
