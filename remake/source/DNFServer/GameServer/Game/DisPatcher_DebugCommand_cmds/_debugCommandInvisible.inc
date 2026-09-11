// ===========================================================================
// 13. _debugCommandInvisible —— 0x0859058a
// ===========================================================================
bool DisPatcher_DebugCommand::_debugCommandInvisible(CUser* user,
                                                     MSG_DEBUG_COMMAND& /*msg*/)
{
    user->SetInvisible();                                       // +0x0e SetInvisible()
    PacketGuard packet;                                         // -0x24
    packet.put_header(0, 6);                                    // +0x34 header(0,6)
    packet.put_short(user->get_unique_id() & 0xffff);           // +0x51 put_short(uid&0xffff)
    packet.finalize(true);                                      // +0x64 finalize
    G_GameWorld()->send_all_excepted_user(packet, user);        // +0x7f 广播排除自己
    user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),    // +0xa2 SendNotiPacket(1,2,0)
                         static_cast<ENUM_NOTIPACKET>(2), 0);
    // P227 全表达式临时链：ORIG @0x0859065x 在 cMyTrace ctor 前右起求 varargs
    // 并以 %ebx 跨 ctor 保活；具名 name 局部（旧 register 缓存形）+ 具名 trace
    // 对象反使 packet 槽平移（b28c-lane12）。
    cMyTrace(__PRETTY_FUNCTION__, 0x5c, 0)(
        "GM(%s) is invisible \n", user->getCurCharacName());
    return true;
}