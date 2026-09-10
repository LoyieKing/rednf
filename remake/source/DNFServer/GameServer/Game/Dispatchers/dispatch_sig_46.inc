int Dispatcher_QuickJoinRoom::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820350e [0x0820350e, 0x082037f9]
    unsigned char kind = 0;                                  // -0x19
    if (!packet.get_byte(kind))
    {
        return LineFunc(0x9a32,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->isCompetitionMercenary())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x65, 0x15);
        return 0;
    }
    if (kind == 0)
    {
        PvP_Room* room = G_CGameManager()->FindQuickJoinablePvPRoom(true, user);
        if (room == 0)
        {
            char msgBuf[0x3d];                               // MSG_MAKE_PVP_ROOM（不完整类型，偏移视图）
            memset(msgBuf, 0, 0x3d);
            *reinterpret_cast<unsigned char*>(msgBuf + 0x0d) = 1;
            *reinterpret_cast<unsigned short*>(msgBuf + 0x2c) = 1;
            if (!MakePVPRoom(user, *reinterpret_cast<MSG_MAKE_PVP_ROOM*>(msgBuf),
                             (ENUM_CMDPACKET)0x65, (PVP_BATTLE_MODE)2))
            {
                return 0;
            }
        }
        else
        {
            char pass[9];                                    // -0x22..-0x1a（空串）
            memset(pass, 0, sizeof(pass));
            if (!EnterPVPRoom(user, room, (ENUM_CMDPACKET)0x65, pass))
            {
                return 0;
            }
        }
    }
    else if (kind == 1)
    {
        PacketGuard guard;
        guard.put_header(1, 0x65);
        guard.put_byte(0);
        guard.put_byte(4);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    else if (kind == 2)
    {
        CEventBase* repeat = GlobalData::s_event_manager->GetRepeatEvent(0x5f);
        if (!repeat->IsEventing(0))                          // ORIG 虚调 vptr+0x34 传 NULL
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x65, 8);
            return 0;
        }
        BlueMarble* marble = G_CGameManager()->findJoinableBlueMarble();
        if (marble == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x65, 0xbe);
            return 0;
        }
        int err = marble->enterBlueMarble(user);
        if (0 < err)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x65, (unsigned char)err);
            return 0;
        }
    }
    return 0;
