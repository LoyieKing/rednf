// ===========================================================================
// 18. _debugCommandInitUserPowerwarPoint —— 0x08590092（清零个人 Power War 点）
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandInitUserPowerwarPoint(
    CUser* user, MSG_DEBUG_COMMAND& /*msg*/)
{
    if (!user->isGMUser())
        return false;

    user->DecreasePowerWarPoint(user->GetUserPowerWarPoint());  // +0x27/+0x3c 直扩实参
    PacketGuard packet;                                         // -0x14
    packet.put_header(0, 2);                                    // +0x5c header(0,2)
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);                      // +0xd0 send_all(packet,user)
    return true;
}