int DisPatcher_SetWarRoomSeatState::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x08233620：恒定返回 0
