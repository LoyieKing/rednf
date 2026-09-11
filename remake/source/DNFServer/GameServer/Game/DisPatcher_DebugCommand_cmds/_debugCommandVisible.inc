// ===========================================================================
// 14. _debugCommandVisible —— 0x085906ac
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：area/vill/name 全部内联实参
// （ORIG 无槽，P0/P183）+ P227 临时链。
bool DisPatcher_DebugCommand::_debugCommandVisible(CUser* user,
                                                   MSG_DEBUG_COMMAND& /*msg*/)
{
    user->SetVisible();                                         // +0x0e SetVisible()
    if (user->get_state() == 3)                                 // +0x21 村庄态才发提示
        user->SendNotiPacket(static_cast<CUser::eSendTarget>(0),
                             static_cast<ENUM_NOTIPACKET>(2), 0);

    PacketGuard packet;                                         // -0x24
    packet.put_header(0, 0x17);                                 // +0x6c header(0,0x17)
    packet.put_short(user->get_unique_id() & 0xffff);           // +0x89 uid&0xffff
    packet.put_byte(user->getCurCharacVill());                  // +0xa6 vill
    packet.put_byte(user->get_area(false));                     // +0xc8 get_area(false)
    packet.put_short(user->get_posX() & 0xffff);                // +0xf5 posX
    packet.put_short(user->get_posY() & 0xffff);                // +0xf8 posY
    packet.put_byte(user->get_direction());                     // +0x115 direction
    packet.put_byte(user->get_charac_visible_values());         // +0x139 visible values
    packet.finalize(true);                                      // +0x14f finalize
    // ORIG area→%ebx、vill→%esi 均无槽 ⇒ 内联实参（P0/P183）；name 同（%ebx）。
    G_GameWorld()->send_to_area((int)(char)user->getCurCharacVill(),
                                user->get_area(false), packet); // +0x18e send_to_area
    cMyTrace(__PRETTY_FUNCTION__, 0x83, 0)("GM(%s) is visible \n",
        const_cast<char*>(user->getCurCharacName()));
    return true;
}