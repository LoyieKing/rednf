void GameWorld::send_chat_msg(CUser* from, char type, unsigned short target,
                              unsigned int characNo, char* msg, int len, char* nick,
                              int nickLen, ENUM_NOTIPACKET notify)
{
    // [GameWorld-sweep] P196 符号性直证：ORIG 0x086c9774 `movsbl -0x1ec(%ebp),%eax;
    // cmp $0x1e,%eax; ja` —— 符号扩展到 int 后**无符号**比较 ⇒ 原码是
    // `(unsigned int)type < 0x1f`（写裸 `type < 0x1f` 生 `cmpb $0x1e; jg` 有符号
    // 形，type≥0x80 时语义分叉）。
    if ((unsigned int)type < 0x1f &&
        ((1 << (type & 0x1f)) & 0x7fff0411U) != 0 && from != 0)
    {
        from->DisConnSig((DISCONN_SIG)0x18, 1, 0);
        return;
    }
    if (type != 0)
    {
        std::string filtered = CSyncSlangFilter::GetInstance()->Filter(std::string(msg));
        // [b28d-sweep] ORIG 0x086c985b 段 `test %eax,%eax; sete %al; test %al,%al;
        // je <body>` = 空判定正极性（P197④ 镜像族）⇒ 写 `size()==0` 早退。
        if (filtered.size() == 0)
        {
            return;
        }
        strncpy(msg, filtered.c_str(), filtered.size());
    }
    PacketGuard packet;
    if (from != 0)
    {
        if (IsForbiddenChat(from->getCurCharacName()))
        {
            // [A11-round5 C07/N18b 负例 · 实测已回退] ORIG 0x086c9a4c 段为
            // `call GetLeftTimeOfForbiddenChat; mov %eax,-0x50(%ebp); cmpl $0x0,-0x50(%ebp); je`；
            // P198① 两句式试点（声明/赋值分开）实测 our_insns 仍 1253（4.4 恒折寄存器直判、
            // 槽不回收）⇒ 判 N18b：该值仅单点消费，**不满足 C07「跨 call 后仍被本函数复用」
            // 前提**（Main 转发 A14 反例同界），勿再试两句式/具名局部形。
            unsigned int left = GetLeftTimeOfForbiddenChat(from->getCurCharacName());
            if (left == 0)
            {
                EnableChat(from->getCurCharacName());
            }
            else
            {
                // [GameWorld-sweep] ORIG 0x086c9989 段在 forbidden 分支**无独立
                // PacketGuard 构造**（唯一 PacketGuard C2 在 0x086c9918，即函数级
                // packet）⇒ 原码复用 packet，不另建 err 对象。
                packet.put_header(0, 0xc);
                packet.put_byte(10);
                packet.put_short(0);
                cMyTrace trace(__PRETTY_FUNCTION__, 0xdf3, 0);
                trace("%s is forbidden to chat for %d msec.\n",
                      from->getCurCharacName(), left);
                packet.put_int(left / 60000);
                packet.finalize(true);
                from->Send(packet);
            }
            return;
        }
    }
    WriteLogChatMsg(from, type, msg);
    switch (type)
    {
    case 0:
        if (from == 0)
        {
            make_packet_chat_msg(packet, 0, msg, len, type, notify);
            send_all(packet);
        }
        else
        {
            // ORIG 0x86c9ac1：type==0 且 from 非空时仅调用 get_acc_id（结果丢弃）。
            from->get_acc_id();
        }
        break;
    case 1:
    case 0x21:
    {
        CUser* to = find_from_world(target);
        if (to == 0)
        {
            // ORIG 0x86ca0b9：跨服私聊监控包（m_chatType/m_characNo/
            // m_recverCharId/m_msgLen/m_msg/m_buddyNameLen/buddy_n_user_id_what；
            // SendPacket 长度 0x3b+len）。
            Packet_Monitor_Other_Channel_Chat pkt;
            pkt.m_chatType = (int)type;   // ORIG 0x86c9b35 case 1 离线分支 +0xa 槽（local_1dd）
            pkt.m_characNo = from->getCurCharacNo();
            pkt.m_recverCharId = characNo;
            pkt.m_msgLen = (unsigned char)len;
            memcpy(pkt.m_msg, msg, len);
            pkt.m_buddyNameLen = (char)nickLen;
            memcpy(pkt.buddy_n_user_id_what, nick, nickLen);
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, 0x3b + len);
            break;
        }
        PacketGuard pm;
        bool black = to->isBlackUser(from->get_charac_no(-1)) ||
                     from->isBlackUser(to->get_charac_no(-1));
        if (black)
        {
            pm.put_header(0, 0x41);
            pm.put_byte((int)type);
            pm.put_byte(0x4d);
            int nameLen = (int)strlen(to->getCurCharacName());
            pm.put_int(nameLen);
            pm.put_str(to->getCurCharacName(), nameLen);
            pm.put_byte(0);
            pm.put_int(len);
            pm.put_str(msg, len);
            pm.finalize(true);
            from->Send(pm);
        }
        else if (GlobalData::s_GM_Manager->IsGm(to->get_acc_id()) == 0)
        {
            // [b28d-solo-GameWorld] P34/P277：ORIG 0x086c9e1b `call G_GameWorld;
            // mov %eax,(%esp); call IsIntegratedPvPBaseChannel`（自由函数形 receiver）。
            if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
                from->GetServerGroup() != to->GetServerGroup())
            {
                pm.put_header(0, 0x41);
                pm.put_byte((int)type);
                pm.put_byte(0x15);
                int nameLen = (int)strlen(to->getCurCharacName());
                pm.put_int(nameLen);
                pm.put_str(to->getCurCharacName(), nameLen);
                pm.put_byte(0);
                pm.put_int(len);
                pm.put_str(msg, len);
                pm.finalize(true);
                from->Send(pm);
            }
            else
            {
                pm.put_header(0, 0x41);
                pm.put_byte((int)type);
                pm.put_byte(0);
                int nameLen = (int)strlen(from->getCurCharacName());
                pm.put_int(nameLen);
                pm.put_str(from->getCurCharacName(), nameLen);
                pm.put_byte(0);
                pm.put_int(len);
                pm.put_str(msg, len);
                pm.finalize(true);
                to->Send(pm);
            }
        }
        else
        {
            pm.put_header(0, 0x41);
            pm.put_byte((int)type);
            pm.put_byte(0x5a);
            int nameLen = (int)strlen(to->getCurCharacName());
            pm.put_int(nameLen);
            pm.put_str(to->getCurCharacName(), nameLen);
            pm.put_byte(0);
            pm.put_int(len);
            pm.put_str(msg, len);
            pm.finalize(true);
            from->Send(pm);
        }
        break;
    }
    case 2:
        if (from != 0)
        {
            make_packet_chat_msg(packet, from, msg, len, type, notify);
            CParty* party = (CParty*)from->GetParty();
            if (party != 0)
            {
                party->send_to_party(packet);
            }
        }
        break;
    case 3:
    {
        // [b28d-solo-GameWorld] ORIG 0x086ca212 `call G_GameWorld; call getUserVillage`
        Village* village = G_GameWorld()->getUserVillage(from);
        if (village == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0xf1d, 5);
            trace("getUserVillage() call failed");
            break;
        }
        // [L11·widthscan T1 直证] ORIG 该局部为 `movl $0x0,-0x31(%ebp)`（32 位清零）
        // ⇒ 原码是 int（我方 bool 产 `movb $0x0`）。
        int needClean = 0;
        if (village->get_gate_area() != from->get_area(false) &&
            from->getCurCharacVill() != 8 && (int)notify != 0x72 && (int)notify != 0x77)
        {
            needClean = 1;
        }
        if (needClean)
        {
            int cleanCount = G_CDataManager()->m_serverParameter.GetCleanChattingCount(
                from->get_charac_level());
            std::string smsg(msg);
            int add = from->isDuplicationMessage(smsg) != 0
                          ? G_CDataManager()->m_serverParameter.GetPaneltyDupMessage()
                          : 1;
            from->incChattingMessageCount(add * cleanCount);
            if (from->getChattingMessageCount() >
                G_CDataManager()->m_serverParameter.GetMaxCleanChattingCount())
            {
                from->setChattingMessageCount(0);
                from->reqHumanCertify4ClearMap(false);
                break;
            }
        }
        // [b28d-sweep] ORIG 跳表 0x8cfba20[3]=0x86ca20b / [0x22]=0x86ca3de：case 3 的
        // 正常路径（needClean 假或 count<=max）**落到** 0x86ca3de，与 case 0x22 共享同一
        // 块（ORIG 全函数仅一次 make_packet_chat_msg/send_to_area，0x86ca41f/0x86ca45f）
        // ⇒ 此处不写 break，fallthrough 到 case 0x22。
    }
    case 0x22:
        if (from != 0)
        {
            make_packet_chat_msg(packet, from, msg, len, type, notify);
            send_to_area(from->getCurCharacVill(), from->get_area(false), packet);
        }
        break;
    case 4:
        make_packet_chat_msg(packet, from, msg, len, type, notify);
        send_all(packet);
        break;
    case 5:
        if (from != 0)
        {
            make_packet_chat_msg(packet, from, msg, len, type, notify);
            if (from->GetAssaultPlace() == 0)
            {
                // [b28d-solo-GameWorld] ORIG 0x086ca552 单次 `call G_CGameManager`
                // + `movswl %ax,%ebx` + GetPvp（无判空三元；p167 EXTRA 实证）。
                PvP_Room* room = G_CGameManager()->GetPvp(
                    (int)from->GetPvpIndex(), from, 0);
                if (room != 0)
                {
                    room->send_to_pvp(packet);
                }
            }
            else
            {
                pvp_assault::GetInstanceAssaultMgr()->SendPacket(from, false, &packet);
            }
        }
        break;
    case 6:
        if (from->getCurCharacNo() != 0 && from->get_charac_guildkey() != 0)
        {
            // ORIG 0x86ca6ac：Packet_Monitor_Guild_Chat（ORIG 实证：+0xa 槽写
            // guildkey、+0xe 槽写 characNo，与头文件字段名相反；SendPacket 长度
            // 0x13+len）。
            Packet_Monitor_Guild_Chat pkt;
            pkt.m_charNo = from->get_charac_guildkey();
            pkt.m_guildKey = from->getCurCharacNo();
            pkt.m_msgLen = (unsigned char)len;
            memcpy(pkt.m_msg, msg, len);
            GlobalData::s_guild_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, len + 0x13);
        }
        else
        {
            LogManager::logFormat(1, "world.cpp",
                __PRETTY_FUNCTION__,
                0xf8c, "GUILD_CHAT_ERROR : charac no == %d || guild key == %d ",
                from->getCurCharacNo(), from->get_charac_guildkey());
        }
        break;
    case 8:
        if (from->getCurCharacNo() != 0 &&
            from->get_charac_memberkey() != 0)
        {
            // ORIG 0x86ca7dc：Packet_Monitor_Member_Chat（SendPacket 长度 0xf+len）。
            Packet_Monitor_Member_Chat pkt;
            pkt.m_memberId = from->get_charac_memberkey();
            pkt.m_msgLen = (unsigned char)len;
            memcpy(pkt.m_msg, msg, len);
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, len + 0xf);
        }
        else
        {
            LogManager::logFormat(1, "world.cpp",
                __PRETTY_FUNCTION__,
                0xfa3, "MEMBER_CHAT_ERROR : member key == %d ",
                (int)from->get_charac_memberkey());
        }
        break;
    case 9:
        if (from != 0)
        {
            make_packet_chat_msg(packet, from, msg, len, type, notify);
            if (from->GetAssaultPlace() == 0)
            {
                // [b28d-solo-GameWorld] ORIG 0x086ca62c 单次 G_CGameManager（同上）
                PvP_Room* room = G_CGameManager()->GetPvp(
                    (int)from->GetPvpIndex(), from, 0);
                if (room != 0)
                {
                    room->send_to_pvp_team(packet, from);
                }
            }
            else
            {
                pvp_assault::GetInstanceAssaultMgr()->SendPacket(from, true, &packet);
            }
        }
        break;
    case 0xb:
    case 0xc:
    case 0xe:
    {
        // ORIG 0x86ca8b8：Packet_Monitor_MegaPhone::set(channel, uniqueId,
        // name, msgLen, msg, type)；SendPacket 长度 0x2e+len。
        Packet_Monitor_MegaPhone pkt;
        pkt.set(G_CEnvironment()->get_channel_no(), from->get_unique_id(),
                from->getCurCharacName(), (unsigned char)len, msg,
                (unsigned char)type);
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(from->GetServerGroup())
            ->SendPacket((char*)&pkt, len + 0x2e);
        break;
    }
    case 0xd:
    case 0xf:
    {
        PacketGuard pm;
        pm.put_header(0, 0x76);
        pm.put_byte((int)type);
        pm.put_byte(G_CEnvironment()->get_channel_no());
        pm.put_short(from->get_unique_id());
        // ORIG 0x086caa08 `put_int(strlen(from->getCurCharacName()))` 内联 + 0x086caa2d
        // 具名 nameLen（%ebx）⇒ 两次独立 strlen/getCurCharacName（p167 MISSING 各 1 实证）。
        pm.put_int((int)strlen(from->getCurCharacName()));
        int nameLen = (int)strlen(from->getCurCharacName());
        pm.put_str(from->getCurCharacName(), nameLen);
        pm.put_int(len);
        pm.put_str(msg, len);
        pm.finalize(true);
        // [b28d-solo-GameWorld] ORIG 0x086caa9c `call G_GameWorld; call send_all`
        G_GameWorld()->send_all(pm);
        break;
    }
    case 0x28:
        if (from != 0)
        {
            make_packet_chat_msg(packet, from, msg, len, type, notify);
            BlueMarble* bm = (BlueMarble*)from->getBlueMarble();
            if (bm != 0)
            {
                bm->sendToBlueMarble(packet);
            }
        }
        break;
    }
