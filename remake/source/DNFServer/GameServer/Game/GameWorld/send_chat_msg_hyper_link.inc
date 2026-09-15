// [b28d-11b] N：帧差 + 位折叠（N6/N82）@0x086cfa10
// [b28d-solo-GameWorld] _ZN9GameWorld24send_chat_msg_hyper_linkEP5CUserR27MSG_ITEM_HYPER_LINK_MESSAGE15ENUM_NOTIPACKET
// verdict=DIFF 1321/1318 N82/N6/N89 封顶（帧 0xa5c vs 0x35c + 位折叠 + 大帧槽组）。
// [A11-round5 轴 B 修复（已落地，净收益）] case 0xb/0xc/0xe/0x23（本文件 5277 行区）
// ORIG 0x086d0af7/0x086d0afc/0x086d0b01 = `call CUserCharacInfo::getCurrCharacBlackCount;
// cmp $0x12c,%eax; seta %al; test %al,%al; jne <case 尾 0x086d0e86>`（**无符号**正极性早退）；
// 旧文 `if (from->getCurrCharacBlackCount() < 0x12d)` 产 `cmp $0x12c; setle`（有符号）。
// 改 `if (from->getCurrCharacBlackCount() > 0x12cu) { break; }` + 裸块后我方与 ORIG 逐条同形；
// 单件 diff 段 2079→**2075（−4 行）**、TU 四数 195/454/4/12 零回退（20:08:16）⇒
// Main 轴 B「反向（unsigned 向 ORIG 对齐）」修法第二例（同 E08）。
// b28d-solo 本轮真修：同 send_chat_msg case 0xd/0xf —— `put_int(strlen(...))` 内联 +
// 具名 nameLen 两次独立 strlen/getCurCharacName（p167 MISSING 各 1 归零）。
// p167 调用点差集（/tmp/tu_GameWorld.o）：MISSING G_GameWorld×3 / getCurCharacName×1 /
// strlen×1；EXTRA G_CGameManager×2 / basic_string D1×1（同 P34/P277 族 receiver 形 +
// 巨件分支调用点未逐 case 收敛，语义待三件套复验）。
// [H8B 2026-09-11 复核·终判] 1321/1322（我方多 1）+ p167 calls 198/199：同族别名对
// （_ZNSsD2Ev -6 / _ZNSsD1Ev +7、PacketGuard C2/D2 与 C1/D1、Packet_*_Hyper_Link C2/C1、
// cMyTrace C2/C1）⇒ 无真缺口。残差 N82（帧 0xa5c vs 0x35c 大帧槽组）/N6（位折叠）/N89。
// [A11-round5] case 0xb/0xc/0xe/0x23 的 unsigned 正极性早退（ORIG `cmp $0x12c; seta;
// jne <case 尾>`）既有真修保留（diff 2079→2075）。
void GameWorld::send_chat_msg_hyper_link(CUser* from,
                                        MSG_ITEM_HYPER_LINK_MESSAGE& msg,
                                        ENUM_NOTIPACKET notify)
{
    if (msg.m_typeD < 0x1f && ((1 << (msg.m_typeD & 0x1f)) & 0x7fff0411U) != 0 &&
        from != 0)
    {
        from->DisConnSig((DISCONN_SIG)0x18, 1, 0);
        return;
    }
    if (msg.m_typeD != 0)
    {
        // [R14a21] ORIG 0x86cfaa3-0x86cfbac 实证：源串为**具名局部**（C2EPKcRKSaIcE 建于
        // -0x90，析构在 scope 尾 0x86cfbac；Filter 结果 -0x94 先析构 0x86cfb6e）——
        // 匿名临时（`Filter(std::string(...))`）只活到整表达式结束，会提前析构并使
        // 该字符串的 EH landing pad 多一份（p167 EXTRA _ZNSsD2Ev +1）。
        std::string srcText(msg.m_text18);
        std::string filtered = CSyncSlangFilter::GetInstance()->Filter(srcText);
        if (filtered.size() != 0)
        {
            strncpy(msg.m_text18, filtered.c_str(), filtered.size());
        }
        else
        {
            return;
        }
    }
    PacketGuard packet;
    WriteLogChatMsg(from, msg.m_typeD, msg.m_text18);
    switch (msg.m_typeD)
    {
    case 0:
    case 7:
    case 10:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:
    case 0x20:
    case 0x21:
    case 0x24:
    case 0x25:
    case 0x26:
        break;
    case 1:
    {
        CUser* to = find_from_world(msg.m_itemSpaceE);
        if (to == 0)
        {
            // ORIG 0x86d01c5：Packet_Monitor_Other_Channel_Chat_Hyper_Link
            // （m_chatType/m_characNo/m_recverCharId/m_msgLen/m_msg/
            //  m_buddyNameLen/buddy/m_itemCount/m_items；SendPacket 长度
            //  0x174+textLen）。
            Packet_Monitor_Other_Channel_Chat_Hyper_Link pkt;
            pkt.m_chatType = msg.m_typeD;   // ORIG 0x86cfa10 case 1 离线分支 +0xa 槽（local_a15）
            pkt.m_characNo = from->getCurCharacNo();
            pkt.m_recverCharId = msg.m_slot10;
            pkt.m_msgLen = (unsigned char)msg.m_textLen14;
            memcpy(pkt.m_msg, msg.m_text18, msg.m_textLen14);
            pkt.m_buddyNameLen = (char)msg.m_nameLen118;
            memcpy(pkt.buddy_n_user_id_what, msg.m_name11c, msg.m_nameLen118);
            pkt.m_itemCount = msg.m_linkCount13b;
            for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
            {
                memcpy(pkt.m_items[i], msg.m_links13c[i], 0x68);
            }
            // ORIG 0x86d02e0 `add $0x174,%ax` + `movzwl %ax,%eax`：长度经 unsigned short
            // 具名局部**截断**（P171 原码行为照抄，非 32 位加）。
            unsigned short packetSize = (unsigned short)(msg.m_textLen14 + 0x174);
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, packetSize);
            break;
        }
        PacketGuard pm;
        bool black = to->isBlackUser(from->get_charac_no(-1)) ||
                     from->isBlackUser(to->get_charac_no(-1));
        if (black)
        {
            pm.put_header(0, 0x173);
            pm.put_byte(msg.m_typeD);
            pm.put_byte(0x4d);
            int nameLen = (int)strlen(to->getCurCharacName());
            pm.put_int(nameLen);
            pm.put_str(to->getCurCharacName(), nameLen);
            pm.put_byte(0);
            pm.put_int(msg.m_textLen14);
            pm.put_str(msg.m_text18, msg.m_textLen14);
            pm.put_byte(msg.m_linkCount13b);
            for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
            {
                pm.put_binary((char*)msg.m_links13c[i], 0x68);
            }
            pm.finalize(true);
            from->Send(pm);
        }
        // [b28d-solo-GameWorld] P34/P277：ORIG 自由函数形 receiver（hyper_link 同族）。
        else if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
                 from->GetServerGroup() != to->GetServerGroup())
        {
            // ORIG 0x86cfe74（put_byte 0x15）：跨服私聊超链接，用接收方名，
            // 包回发给发送方。Ghidra C 与汇编均无普通版同处的 GM(0x5a) 分支。
            pm.put_header(0, 0x173);
            pm.put_byte(msg.m_typeD);
            pm.put_byte(0x15);
            int nameLen = (int)strlen(to->getCurCharacName());
            pm.put_int(nameLen);
            pm.put_str(to->getCurCharacName(), nameLen);
            pm.put_byte(0);
            pm.put_int(msg.m_textLen14);
            pm.put_str(msg.m_text18, msg.m_textLen14);
            pm.put_byte(msg.m_linkCount13b);
            for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
            {
                pm.put_binary((char*)msg.m_links13c[i], 0x68);
            }
            pm.finalize(true);
            from->Send(pm);
        }
        else
        {
            pm.put_header(0, 0x173);
            pm.put_byte(msg.m_typeD);
            pm.put_byte(0);
            int nameLen = (int)strlen(from->getCurCharacName());
            pm.put_int(nameLen);
            pm.put_str(from->getCurCharacName(), nameLen);
            pm.put_byte(0);
            pm.put_int(msg.m_textLen14);
            pm.put_str(msg.m_text18, msg.m_textLen14);
            pm.put_byte(msg.m_linkCount13b);
            for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
            {
                pm.put_binary((char*)msg.m_links13c[i], 0x68);
            }
            pm.finalize(true);
            to->Send(pm);
        }
        break;
    }
    case 2:
        if (from != 0)
        {
            make_packet_chat_msg_hyper_link(packet, from, msg, notify);
            CParty* party = (CParty*)from->GetParty();
            if (party != 0)
            {
                party->send_to_party(packet);
            }
        }
        break;
    case 3:
    case 0x27:
    {
        Village* village = G_GameWorld()->getUserVillage(from);
        if (village == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1934, 5);
            trace("getUserVillage() call failed");
            break;
        }
        // 同上（widthscan T1 直证）：ORIG `movl $0x0,-0xNN` ⇒ int。
        int needClean = 0;
        if (village->get_gate_area() != from->get_area(false) &&
            from->getCurCharacVill() != 8 && (int)notify != 0x72)
        {
            needClean = 1;
        }
        if (needClean)
        {
            int cleanCount =
                G_CDataManager()->m_serverParameter.GetCleanChattingCount(
                    from->get_charac_level());
            std::string smsg(msg.m_text18);
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
    }
    case 0x22:
        if (from != 0)
        {
            make_packet_chat_msg_hyper_link(packet, from, msg, notify);
            send_to_area(from->getCurCharacVill(), from->get_area(false), packet);
        }
        break;
    case 4:
        make_packet_chat_msg_hyper_link(packet, from, msg, notify);
        send_all(packet);
        break;
    case 5:
        if (from != 0)
        {
            make_packet_chat_msg_hyper_link(packet, from, msg, notify);
            if (from->GetAssaultPlace() == 0)
            {
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
            // ORIG 0x86d06ac：Packet_Monitor_Guild_Chat_Hyper_Link（同普通版
            // 0x86ca6ac 怪癖：+0xa 槽(local_53a)先写 guildkey、+0xe 槽(local_536)
            // 后写 characNo，与头文件字段名相反；SendPacket 长度 0x14c+textLen）。
            Packet_Monitor_Guild_Chat_Hyper_Link pkt;
            // [P-GPT] 头已按 ORIG 校正成员名（m_guildKey@+0xa / m_charNo@+0xe）⇒ 直接按名字写，
            //         写入的字节偏移与改名前逐个相同（+0xa 收 guildkey、+0xe 收 characNo）。
            pkt.m_guildKey = from->get_charac_guildkey();
            pkt.m_charNo = from->getCurCharacNo();
            pkt.m_itemCount = msg.m_linkCount13b;
            for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
            {
                memcpy(pkt.m_items + i * 0x68, msg.m_links13c[i], 0x68);
            }
            pkt.m_msgLen = (unsigned char)msg.m_textLen14;
            memcpy(pkt.m_msg, msg.m_text18, msg.m_textLen14);
            // ORIG 0x86d08c5 `add $0x14c,%ax`：unsigned short 具名局部截断（同 0x174 处）。
            unsigned short packetSize = (unsigned short)(msg.m_textLen14 + 0x14c);
            GlobalData::s_guild_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, packetSize);
        }
        else
        {
            LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x19b8,
                "GUILD_CHAT_HYPER_LINK_ERROR : charac no == %d || guild key == %d ",
                from->getCurCharacNo(), from->get_charac_guildkey());
        }
        break;
    case 8:
        if (from->getCurCharacNo() != 0 &&
            from->get_charac_memberkey() != 0)
        {
            Packet_Monitor_Member_Chat_Hyper_Link pkt;
            pkt.m_memberId = from->get_charac_memberkey();
            pkt.m_itemCount = msg.m_linkCount13b;
            for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
            {
                memcpy(pkt.m_items[i], msg.m_links13c[i], 0x68);
            }
            pkt.m_msgLen = (unsigned char)msg.m_textLen14;
            memcpy(pkt.m_msg, msg.m_text18, msg.m_textLen14);
            // ORIG 0x86d0a5e `add $0x148,%ax`：unsigned short 具名局部截断（同 0x174 处）。
            unsigned short packetSize = (unsigned short)(msg.m_textLen14 + 0x148);
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, packetSize);
        }
        else
        {
            LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x19d6,
                "MEMBER_CHAT_HYPER_LINK_ERROR : member key == %d ",
                (int)from->get_charac_memberkey());
        }
        break;
    case 9:
        if (from != 0)
        {
            make_packet_chat_msg_hyper_link(packet, from, msg, notify);
            if (from->GetAssaultPlace() == 0)
            {
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
    case 0x23:
        // [A11-round5 轴 B（P196 朝 ORIG 方向）] ORIG 0x086d0af7/0x086d0afc/0x086d0b01：
        // `call CUserCharacInfo::getCurrCharacBlackCount; cmp $0x12c,%eax; seta %al;
        // test %al,%al; jne <case 尾 0x086d0e86>` = **无符号**正极性早退 `if (X > 0x12c) break;`
        // （我方原 `< 0x12d` 产 `cmp $0x12c; setle` 有符号序）。改无符号常量 + 早退形。
        if (from->getCurrCharacBlackCount() > 0x12cu)
        {
            break;
        }
        {
            Packet_Monitor_MegaPhone_Hyper_Link pkt;
            pkt.set(G_CEnvironment()->get_channel_no(), from->get_unique_id(),
                    from->getCurCharacName(), (unsigned char)msg.m_textLen14,
                    msg.m_text18, msg.m_typeD, msg.m_linkCount13b,
                    (const hyperlink_item_info*)msg.m_links13c[0]);
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, msg.m_textLen14 + 0x167);
        }
        break;
    case 0xd:
    case 0xf:
    {
        PacketGuard pm;
        pm.put_header(0, 0x174);
        pm.put_byte((int)msg.m_typeD);
        pm.put_byte(G_CEnvironment()->get_channel_no());
        pm.put_short(from->get_unique_id());
        // ORIG 0x086d0xxx 同 send_chat_msg case 0xd/0xf：`put_int(strlen(...))` 内联
        // + 具名 nameLen ⇒ 两次独立 strlen/getCurCharacName（p167 MISSING 各 1 实证）。
        pm.put_int((int)strlen(from->getCurCharacName()));
        int nameLen = (int)strlen(from->getCurCharacName());
        pm.put_str(from->getCurCharacName(), nameLen);
        pm.put_int(msg.m_textLen14);
        pm.put_str(msg.m_text18, msg.m_textLen14);
        pm.put_byte(msg.m_linkCount13b);
        for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
        {
            pm.put_binary((char*)msg.m_links13c[i], 0x68);
        }
        pm.finalize(true);
        G_GameWorld()->send_all(pm);
        break;
    }
    case 0x28:
        if (from != 0)
        {
            make_packet_chat_msg_hyper_link(packet, from, msg, notify);
            BlueMarble* bm = (BlueMarble*)from->getBlueMarble();
            if (bm != 0)
            {
                bm->sendToBlueMarble(packet);
            }
        }
        break;
    default:
        break;
    }
}