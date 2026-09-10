bool DisPatcher_DebugCommand::_debugCommandOn(CUser* user,
                                              MSG_DEBUG_COMMAND& /*msg*/)
{
    DBG_TRACE(g_dbgCmdPrettyOn, 0x10, "%s:'%s' GM Mode ON.");   // +0x2b
    user->SetGameMasterMode(true);              // +0x7e SetGameMasterMode(1)

    // ORIG `test %eax,%eax; setne %al; test %al,%al; je <尾 mov $0>` ⇒ 非零才进主流程
    // （正向包裹，P222/P214①；负极性 `==0 return false` 反产 sete 形）。
    if (user->getCurCharacR() != 0)             // +0x83 getCurCharacR!=0
    {
        PacketGuard packet;                     // -0x24 local_28
        packet.put_header(0, 0xe);              // +0xbc put_header(0,0xe)
        packet.put_byte(0);                     // +0xcf put_byte(0)
        packet.put_short(3);                    // +0xe2 put_short(3)
        if (!user->getCurCharacInvenR()->MakeItemPacket(        // +0x10c slot 1
                static_cast<INVEN_TYPE>(1), 1, packet))
            return false;
        if (!user->getCurCharacInvenR()->MakeItemPacket(        // +0x147 slot 0
                static_cast<INVEN_TYPE>(1), 0, packet))
            return false;
        if (!user->getCurCharacInvenR()->MakeItemPacket(        // +0x17f slot 2
                static_cast<INVEN_TYPE>(1), 2, packet))
            return false;
        packet.finalize(true);                  // +0x1a0 finalize(true)
        user->Send(packet);                     // +0x1b2 Send
        return true;                            // +0x1b7 ret 1
    }
    return false;
