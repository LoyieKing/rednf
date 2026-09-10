int Dispatcher_Break_Away_Quest_Check::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG（帧 $0x18）无 req 绑定槽 ⇒ 内联表达式直算地址。
    if (!packet.get_byte(*reinterpret_cast<char*>(reinterpret_cast<char*>(&msg) + 0xd)))
    {
        return LineFunc(0x3197,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
