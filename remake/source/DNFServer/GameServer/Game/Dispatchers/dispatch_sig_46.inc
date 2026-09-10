int Dispatcher_QuickJoinRoom::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820350e [0x0820350e, 0x082037f9]
    // ORIG 0x0820350e 段无 kind 预清零（无 `movb $0x0,-0x19`）⇒ P0/P328⑤ 去初始化。
    unsigned char kind;                                      // -0x19
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
            // ORIG 段直证：`movb $0x1,-0x6c(%ebp)` / `movw $0x1,-0x4d(%ebp)` —— 常量下标
            // 折叠为 ebp 直接偏移（原 `*(T*)(buf+0xNN)` 形退化为 lea+add 两段）。
            msgBuf[0x0d] = 1;
            *reinterpret_cast<unsigned short*>(&msgBuf[0x2c]) = 1;
            if (!MakePVPRoom(user, *reinterpret_cast<MSG_MAKE_PVP_ROOM*>(msgBuf),
                             (ENUM_CMDPACKET)0x65, (PVP_BATTLE_MODE)2))
            {
                return 0;
            }
        }
        else
        {
            // ORIG 段直证：`movl $0x0,-0x22; movl $0x0,-0x1e; movb $0x0,-0x1a`（9B 聚合
            // 零初始化内联；原 memset 形在 4.4 退化为 call）。
            char pass[9] = { 0 };                            // -0x22..-0x1a（空串）
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
        // ORIG 段直证：`mov (%edx),%edx; add $0x34,%edx`（寄存器链，无 repeat 槽）⇒
        // P0/P183 内联指针（原具名 repeat 落 -0x14 槽）。
        if (!GlobalData::s_event_manager->GetRepeatEvent(0x5f)->IsEventing(0))  // ORIG 虚调 vptr+0x34 传 NULL
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
