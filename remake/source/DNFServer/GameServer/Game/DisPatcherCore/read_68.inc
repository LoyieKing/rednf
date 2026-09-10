int DisPatcher_PvpHeartBeat::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)

{
    return 0;    // ORIG 0x08233760：恒定返回 0
}
