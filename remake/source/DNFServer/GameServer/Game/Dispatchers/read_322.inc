// ==== Dispatcher_StackableActionUse::read (ORIG 0x0825df72) ====
int Dispatcher_StackableActionUse::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_STACKABLE_ACTION_USE& req = reinterpret_cast<MSG_STACKABLE_ACTION_USE&>(msg);
    if (!packet.get_short(req.m_0d))                     // +0x0d slot
    {
        return LineFunc(251,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0f)))        // +0x0f invenType
    {
        return LineFunc(254,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_10))                       // +0x10 checksum
    {
        return LineFunc(257,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_14)))        // +0x14 type
    {
        return LineFunc(260,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_15))                       // +0x15 param1
    {
        return LineFunc(263,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_19))                       // +0x19 param2
    {
        return LineFunc(266,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}