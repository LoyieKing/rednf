// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x0820350e orig/ours=182/191 ndiff-blocks=12
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x90,%esp | movl   $&_ZZN24Dispatcher_QuickJoinRoom12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | lea    -0x79(%ebp),%eax
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
            // [DispG1/46] ORIG 0x082035dd 先写 +0x2c（movw $1,-0x4d）、0x082035e3 后写
            //   +0x0d（movb $1,-0x6c）；两处均常量折叠为 ebp 直接偏移（无 lea）⇒
            //   P183/P30：裸常量下标，不用 packed 视图（视图指针形多发 1 条 lea）。
            // [DispG1/46 负结论 2026-09-11] 实测裸下标形 182→**183**（劣化 1）⇒ 回滚视图形。
            msgBuf[0x0d] = 1;
            struct __attribute__((packed)) MsgMakePvPRoomView
            {
                char m_pad0[0x2c];
                unsigned short m_field2c;   // +0x2c
            };
            reinterpret_cast<MsgMakePvPRoomView*>(msgBuf)->m_field2c = 1;
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
}