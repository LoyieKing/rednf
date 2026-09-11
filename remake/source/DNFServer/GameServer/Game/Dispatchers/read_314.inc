// ==== Dispatcher_BlueMarbleConfirmInfo::read (ORIG 0x081e6242) ====
int Dispatcher_BlueMarbleConfirmInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BLUE_MARBLE_CONFIRM_INFO& req =
        reinterpret_cast<MSG_BLUE_MARBLE_CONFIRM_INFO&>(msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))        // +0x0d flag
    {
        return LineFunc(24845,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_0e)))    // +0x0e index
    {
        return LineFunc(24848,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}