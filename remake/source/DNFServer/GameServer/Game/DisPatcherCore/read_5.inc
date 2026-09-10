int DisPatcher_SelectCharac::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_CHARAC_SELECT& m = *reinterpret_cast<MSG_CHARAC_SELECT*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_slot)))  // +0x0d（signed char→char& 适配）
    {
        return LineFunc(0x156f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
