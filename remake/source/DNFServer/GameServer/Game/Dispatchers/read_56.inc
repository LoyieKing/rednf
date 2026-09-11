int Dispatcher_ScanBotByDrv::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x082646ae：读 int(msg+0xd) 失败记 LineFunc(0x42c)；
    // 再读 byte(msg+0x11)（signed char）失败记 LineFunc(0x431)。
    if (!packet.get_int(*reinterpret_cast<int*>((char*)&msg + 0xd)))
    {
        return LineFunc(0x42c, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>((char*)&msg + 0x11)))
    {
        return LineFunc(0x431, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}