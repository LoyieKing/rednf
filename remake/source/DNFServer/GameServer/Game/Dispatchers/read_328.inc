// ==== Dispatcher_RequestIngameAdvertisement::read (ORIG 0x081e434c) ====
int Dispatcher_RequestIngameAdvertisement::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG（帧 $0x28）：msg 指针先物化 -0xc 再回读算址 ⇒ 命名局部。
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<short*>(p + 0xd)))
    {
        return LineFunc(
            0x5c10,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
}