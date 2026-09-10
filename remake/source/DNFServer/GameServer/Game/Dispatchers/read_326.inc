int Dispatcher_ObjectBringUp::read(PacketBuf& packet, MSG_BASE& msg)
{
    // PacketBuf::get_short(unsigned short&)（零扩展口径）
    // ORIG 0x081e147e（帧 $0x28）：msg 指针先物化 -0xc 再回读算址 ⇒ 命名局部。
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(p + 0xe)))
    {
        return LineFunc(
            0x56aa,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
