int Dispatcher_StartGame::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x813f7b0：读 0xd 字节（msg 引用先存 -0xc）
    MSG_BASE& m = msg;
    packet.get_binary((char*)&m, 0xd);
    return 0;
