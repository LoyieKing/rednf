int DisPatcher_RequestGuildWarInfo::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)

{
    return 0;    // ORIG 0x0823374c：恒定返回 0
}
