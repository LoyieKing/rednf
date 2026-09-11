// ORIG 0x081d38ba：get_byte 读入 msg.m_checkFlag（+0xd）；失败走
// LineFunc(0x3197) 返回其值。
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
}