int DisPatcher_QuestAction::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2)
    {
        return 0;
    }
    if (G_GameWorld()->IsPvPSkilTreeChannel() != false)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(packet.at(1)), 1);
        return 0;
    }
    short cmd = 0;
    // [G5-DisPatcherCore] P196/T1：ORIG 0x081ee0xx `movzbl %al,%edx; movzwl -0x22(%ebp),%eax;
    // cwtl; cmp %eax,%edx` = **int 32 位比较**（at(1) 零扩展、cmd 符号扩展）；我方原写
    // `static_cast<unsigned short>(cmd)` 使其折成 `cmp %ax,%dx` 16 位比较（宽度/符号性错位，
    // 语义对 at1∈[0,255] 等价但机器形错）。去掉 cast 即恢复整型提升形。
    // 否定形 `== false` 产 `xor $1;test`（4 条），ORIG 为裸 `test %al;je`（P292 正形）⇒ 改 `!`。
    if (!packet.get_short(cmd) || (packet.at(1) & 0xff) == cmd)
    {
        unsigned char bVar3 = packet.at(1);
        if (bVar3 > 0x20)
        {
            if (bVar3 < 0x23)
            {
                short local_28;
                if (packet.get_short(local_28) != true)
                {
                    LineFunc(0x7149,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                int local_24 = reinterpret_cast<int>(G_CDataManager()->find_quest(static_cast<int>(local_28)));
                if (local_24 == 0)
                {
                    return 0;
                }
                // [G5-DisPatcherCore] P0/P329：ORIG 0x081ee2xx `call at; movzbl %al,%eax;
                // cmp $0x21,%eax; jne` —— 判定值直用 %eax（无 char 具名槽），我方原写
                // `char cVar1 = static_cast<char>(packet.at(1));` 多一个 1 字节槽与往返。
                if (static_cast<char>(packet.at(1)) == '!' &&
                    GlobalData::s_timeGate_->setBlindTimeGateStateQuest(static_cast<int>(local_28)) != false)
                {
                    user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x21), 8);
                }
                user->quest_action(static_cast<int>(packet.at(1) & 0xff), static_cast<int>(local_28), 0, 0);
            }
            else if (bVar3 == 0x23)
            {
                if (user->get_state() == 5)
                {
                    CParty* local_20 = static_cast<CParty*>(user->GetParty());
                    if (local_20 == 0)
                    {
                        short sVar5 = user->GetPartyIndex();
                        unsigned int uVar9 = user->get_acc_id();
                        char* uVar8 = NumberToString(uVar9, 0);
                        LogManager::logFormat
                                  (1, "PacketDispatcher_Impl_1.cpp",
                                   __PRETTY_FUNCTION__,
                                   0x71af, "<IN_DUNGEON_NULL_PARTY> error m_id(%s), PartyIndex(%d)", uVar8,
                                   static_cast<int>(sVar5));
                        return 0;
                    }
                    if (local_20->m_battleField.m_bloodState == 1)   // CParty.h:764 int m_battleField.m_bloodState@+0xcd8
                    {
                        return 0;
                    }
                }
                short local_4c;
                if (packet.get_short(local_4c) != true)
                {
                    LineFunc(0x71b9,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                char local_4a;
                if (packet.get_byte(local_4a) != true)
                {
                    LineFunc(0x71ba,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                char local_49[14];
                local_49[0] = '\0';
                if ((((local_4a == '\x10') || (local_4a == ' ')) || (local_4a == '@')) &&
                    (packet.get_byte(local_49[0]) != true))
                {
                    LineFunc(0x71c4,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                user->quest_action(static_cast<int>(packet.at(1) & 0xff), static_cast<int>(local_4c),
                                   static_cast<int>(local_4a), static_cast<int>(local_49[0]));
            }
            else if (bVar3 == 0x24)
            {
                short local_3b;
                if (packet.get_short(local_3b) != true)
                {
                    LineFunc(0x719a,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                short local_39;
                if (packet.get_short(local_39) != true)
                {
                    LineFunc(0x719b,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                short local_37[7];
                if (packet.get_short(local_37[0]) != true)
                {
                    LineFunc(0x719c,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                if (0 < local_37[0])
                {
                    user->quest_action(static_cast<int>(packet.at(1) & 0xff), static_cast<int>(local_3b),
                                       static_cast<int>(local_39), static_cast<int>(local_37[0]));
                    user->setGmQuestFlag(false);
                }
            }
        }
    }
    else
    {
        // 0x81ee0b4..0x81ee130：get_short 成功但 cmd 与 at(1) 不一致 → 伪造命令，hack 记数 + 错误包
        WongWork::CHackAnalyzer* analyzer = user->getHackAnalyzer();
        analyzer->addServerHackCnt(user, static_cast<WongWork::ENUM_HACKTYPE>(0x333), 1, 0, 0);
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(packet.at(1)), 1);
    }
    return 0;
