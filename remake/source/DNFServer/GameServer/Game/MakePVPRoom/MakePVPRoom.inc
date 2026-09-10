bool MakePVPRoom(CUser* user, MSG_MAKE_PVP_ROOM& msg, ENUM_CMDPACKET cmd, PVP_BATTLE_MODE mode)
{
    PacketGuard packet;                                  // -0x40
    packet.put_header(1, (int)cmd);                      // 0x81f7968

    // 最高段位频道的段位上限门（ORIG 0x81f796d..0x81f7a1d：ch==0xf || ch==0x10，
    // ORIG 短路两次 G_GameWorld()/GetChannelType() 不缓存 —— P48 去缓存）
    if (G_GameWorld()->GetChannelType() == 0xf || G_GameWorld()->GetChannelType() == 0x10)
    {
        if (G_CDataManager()->GetMaxGradePvPChannel() < user->get_pvp_grade())
        {
            packet.put_byte(0);                          // 0x81f79c8..0x81f7a13
            packet.put_byte(0xd);
            packet.finalize(1);
            user->Send(packet);
            return false;
        }
    }

    // 公平 PvP 给品计数门（ORIG 0x81f7a1d..0x81f7a62：give <= get_pvp_item_give → err 7）
    // P0 槽-局部律：ORIG 无槽往返（GetGiveItem 结果留 %esi、limit 留 %eax 直比）⇒ 全内联右值
    if ((int)((fair_pvp::CFairPvPScore*)user->GetCharacExpandData(
                  (ENUM_CHARAC_EXPAND_TYPE)7))->GetGiveItem()
        <= G_CDataManager()->m_serverParameter.get_pvp_item_give(user->get_pvp_grade()))
    {
        packet.put_byte(0);                              // 0x81f7a64..0x81f7aaf
        packet.put_byte(0x7);
        packet.finalize(1);
        user->Send(packet);
        return false;
    }

    // 幽灵状态门（ORIG 0x81f7ab9..0x81f7b13）
    if (user->IsCurCharacGhost() != 0)
    {
        packet.put_byte(0);
        packet.put_byte(0x13);
        packet.finalize(1);
        user->Send(packet);
        return false;
    }

    // 非 PvP 频道门（ORIG 0x81f7b1d..0x81f7b81：!IsPVPChannel() → err 0x13）
    if (!G_GameWorld()->IsPVPChannel())
    {
        packet.put_byte(0);
        packet.put_byte(0x13);
        packet.finalize(1);
        user->Send(packet);
        return false;
    }

    // 用户状态门（ORIG 0x81f7b86..0x81f7be6：get_state() != 3 → err 0x13）
    if (user->get_state() != 3)
    {
        packet.put_byte(0);
        packet.put_byte(0x13);
        packet.finalize(1);
        user->Send(packet);
        return false;
    }

    // PvP 管理器在位门（ORIG 0x81f7bf0..0x81f7c56：GetPvp()==0 → err 4）
    PvP_Room* pvp = G_CGameManager()->GetPvp();         // -0x24
    if (pvp == 0)
    {
        packet.put_byte(0);
        packet.put_byte(0x4);
        packet.finalize(1);
        user->Send(packet);
        return false;
    }

    // 组队邀请转发 + 出队登记（ORIG 0x81f7c5b..0x81f7cb6）
    if (user->CheckInParty())
    {
        CParty* party = (CParty*)user->GetParty();       // -0x1c
        if (party != 0)
        {
            party->send_invite_pvp((int)(unsigned short)user->get_unique_id());
            G_CGameManager()->CheckOutParty(user, false);
        }
    }

    // 建房（ORIG 0x81f7cb7..0x81f7d26）
    int roomNo = pvp->create_room(user, (char)msg.m_field0d, msg.m_name,
                                   (short)msg.m_field2c, msg.m_field2e == 1,
                                   msg.m_field33, msg.m_field2f, mode);   // -0x20

    // pvp@log（ORIG 0x81f7d26..0x81f7db8；0x8583=34179 行号）
    // P227 全表达式临时链：ORIG 在 CSwitchLog ctor 之前右起求 varargs 并以 edi/esi/ebx 跨
    // ctor 保活（get_index→%edi、getCurCharacName→%esi、get_acc_name→%ebx），具名对象+两语句
    // 会退化为 ctor 后求值 ⇒ 用构造即调用链复现。P139：签名串取 __PRETTY_FUNCTION__。
    CSwitchLog(__PRETTY_FUNCTION__, 0x8583, 0, 0)
        ("pvp@log %s,%s, room(%d),%d,%d,%d", user->get_acc_name(),
         user->getCurCharacName(), (int)pvp->get_index(), (int)mode, roomNo, (int)cmd);

    if (roomNo > 0)
    {
        // 成功应答（ORIG 0x81f7dc2..0x81f7e07）
        // 建房失败计费区日志（ORIG 0x81f7e0c..0x81f7e42；0x858c=34188；
        packet.put_byte(0);                              // ORIG 0x81f7dc2..0x81f7e07：byte(0), byte(ret), finalize, Send
        packet.put_byte(roomNo);
        packet.finalize(1);
        user->Send(packet);
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__,
                              0x858c,
                              "create_room fail: PVP\xbd\xbd\xb7\xd4\xb9\xdd\xc8\xaf ID[%d]\n",
                              (int)pvp->get_index());
        G_CGameManager()->PutPvp(pvp);                   // 0x81f7e47..0x81f7e56
        return false;                                    // ebx=0 → 0x81f8025
    }

    // create_room 失败（ret <= 0）：
    // cmd==0x65 附加应答包（ORIG 0x81f7e65..0x81f7ee7）
    if ((int)cmd == 0x65)
    {
        PacketGuard failPacket;                          // -0x4c
        failPacket.put_header(1, 0x65);
        failPacket.put_byte(1);
        failPacket.finalize(1);
        user->Send(failPacket);
    }

    // 基本信息重发（ORIG 0x81f7ef2..0x81f7f78：header(0,2), byte 1, short 1, make_basic_info）
    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(1);
    packet.put_short(1);
    user->make_basic_info((char*)&packet, 1);
    packet.finalize(1);
    user->Send(packet);

    // 房间信息广播（ORIG 0x81f7f7d..0x81f7fea：header(0,0x29), short 1, make_room_info, send_all）
    packet.clear();
    packet.put_header(0, 0x29);
    packet.put_short(1);
    pvp->make_room_info((char*)&packet);
    packet.finalize(1);
    G_GameWorld()->send_all(packet);

    // 传送入 PvP（ORIG 0x81f7fef..0x81f7ffe）
    G_GameWorld()->goto_pvp(user);
    return true;                                         // ebx=1 → 0x81f8025
