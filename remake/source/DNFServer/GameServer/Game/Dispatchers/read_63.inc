int Dispatcher_ChangeEmotion::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 short(msg+0xd)，失败记 LineFunc(0x2e3d)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<short*>(p + 0xd)))
    {
        return LineFunc(0x2e3d, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}