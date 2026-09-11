// ===========================================================================
// 17. _debugCommandUserPowerwarPointUp —— 0x0858fee8
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandUserPowerwarPointUp(
    CUser* user, MSG_DEBUG_COMMAND& msg)
{
    if (!user->isGMUser())
        return false;

    user->IncreasePowerWarPoint((short)msg.m_arg);              // +0x37 Increase(short(msg[0xe]))
    PacketGuard packet;                                         // -0x14
    packet.put_header(0, 2);                                    // +0x5d header(0,2)
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');  // +0x9d make_basic_info(buf,0)
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);                      // +0xcb send_all(packet,user)
    return true;
}