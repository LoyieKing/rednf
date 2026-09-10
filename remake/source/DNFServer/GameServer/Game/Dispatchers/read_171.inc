int Dispatcher_EnterDisjointStore::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 short(msg+0xe)，失败记 LineFunc(0x2aa8)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<short*>(p + 0xe)))
    {
        return LineFunc(0x2aa8, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
