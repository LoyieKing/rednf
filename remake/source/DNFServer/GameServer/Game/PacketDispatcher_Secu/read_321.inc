// ==== Dispatcher_SecurityCardAuthRpy::read (ORIG 0x08262658) ====
int Dispatcher_SecurityCardAuthRpy::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x08262658（报告 Dispatcher_SecurityCardAuthRpy/read.md）：无具名
    // 指针槽——每处 `mov 0x10(%ebp),%eax` 直读参数（帧 0x18）。删中转（技巧36）。
    if (!packet.get_byte(reinterpret_cast<MSG_SECU_RPY*>(&msg)->m_secuType))
        return LineFunc(0x2b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(reinterpret_cast<MSG_SECU_RPY*>(&msg)->m_cryptoType))
        return LineFunc(0x2e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(reinterpret_cast<MSG_SECU_RPY*>(&msg)->m_dataLen))
        return LineFunc(0x32,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_binary(reinterpret_cast<MSG_SECU_RPY*>(&msg)->m_data,
                           reinterpret_cast<MSG_SECU_RPY*>(&msg)->m_dataLen))
        return LineFunc(0x35,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(reinterpret_cast<MSG_SECU_RPY*>(&msg)->m_answerType))
        return LineFunc(0x3a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    return 0;
}