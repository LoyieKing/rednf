int DisPatcher_SetArea::read(PacketBuf& packet, MSG_BASE& msg)

{
    // 载荷字段具名（MSG_SET_USER_AREA，PacketTypes.h:1499 pack(1)；宽度按 ORIG：
    //   get_byteERc @+0xd/+0xe/+0x13 → signed char 字段以 char& 适配；
    //   get_shortERt @+0xf/+0x11/+0x14/+0x16 → unsigned short 字段直引；
    //   081c934b movw $0x0,0x14(%eax) / 081c9354 movw $0x0,0x16(%eax) 两处清零）
    MSG_SET_USER_AREA& req = *reinterpret_cast<MSG_SET_USER_AREA*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_0d)))
    {
        return LineFunc(0x1c16,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_0e)))
    {
        return LineFunc(0x1c17,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_0f))
    {
        return LineFunc(0x1c18,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_11))
    {
        return LineFunc(0x1c19,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_13)))
    {
        return LineFunc(0x1c1a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    req.m_14 = 0;
    req.m_16 = 0;
    if (!packet.get_short(req.m_14))
    {
        return LineFunc(0x1c1f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_16))
    {
        return LineFunc(0x1c20,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
