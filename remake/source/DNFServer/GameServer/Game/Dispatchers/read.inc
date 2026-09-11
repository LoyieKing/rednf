int Dispatcher_UseRightOfChangeGrowType::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x0825dbcc：short@0xd → char@0xf
    MSG_RIGHT_OF_CHANGE_GROW_TYPE& m =
        reinterpret_cast<MSG_RIGHT_OF_CHANGE_GROW_TYPE&>(msg);
    if (!packet.get_short(*reinterpret_cast<short*>(&m.m_0d)))   // +0xd word
        return LineFunc(0x81, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_0f)))      // +0xf byte
        return LineFunc(0x84, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}