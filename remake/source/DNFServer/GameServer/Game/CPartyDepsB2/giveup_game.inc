void CParty::giveup_game(CUser* user, bool a2, bool a3, bool a4)
{
    CBattle_Field& bf = *reinterpret_cast<CBattle_Field*>(&m_battleField);
    // [b28d-solo-CParty2] CParty.h 字段化：+0x1abc 区为具名成员 m_secu。
    Secu_HackLogCheckByParty& secu = m_secu;

    bool specialDungeon = false;                        // -0x3d local_41
    CDungeon* dungeon = m_battleField.m_pDungeon;                      // -0x3c (+0xcac)
    if (dungeon != 0)
    {
        if ((char)dungeon->m_bloodDungeonType > 0)
            specialDungeon = true;
        if (dungeon->isTowerOfDespairDungeon())
            specialDungeon = true;
    }

    bool allowPenalty = a2;                             // -0x15c local_160
    if (dungeon != 0)
    {
        if ((signed char)dungeon->get_dimension_possible() > 0)
            allowPenalty = false;
        else if (dungeon->m_89c != 0)
            allowPenalty = false;
        else if (dungeon->isTournamentDungeon())
            allowPenalty = false;
    }
    if (dungeon != 0)
    {
        if (dungeon->m_entryRestrictFlag != 0)
            allowPenalty = false;
    }
    if (dungeon != 0)
    {
        if (dungeon->m_villageAttackEventFlag != 0)
            allowPenalty = false;
    }

    bool pvpMode = (m_battleField.m_bloodState == 1);                  // -0x35 (+0xcd8)

    user->reset_level_before_dungeon();

    bf.m_dungeonClearTracer.Trace(__PRETTY_FUNCTION__); // 战场 +0xc7c 追踪器

    Reset_party_overlapped_drop_ratio();

    int memberCountSnapshot = get_member_count();       // -0x34（ORIG 存后未读）
    (void)memberCountSnapshot;

    if (user == 0)
        goto AFTER_STATISTICS;
    if (user->getCurCharacR() == 0)
        goto AFTER_STATISTICS;
    if (pvpMode)
        goto AFTER_STATISTICS;
    Statistics::DungeonFail((int)user->getCurCharacR()->m_level);  // characR+0x27
AFTER_STATISTICS:

    for (int i = 0; i <= 3; ++i)                        // -0x24
    {
        if (!_checkValidUser(i))
            continue;
        HistoryLog::WriteGiveUp(
            reinterpret_cast<FILE*>(m_member[i].m_pUser->m_field796f8),
            user->getCurCharacName());
    }

    // 村庄怪物战进行中（bf+0x128≠0）或特殊图 idx==10000 → 取消惩罚资格
    if (bf.m_villageMonsterId != 0)
        allowPenalty = false;
    else if (dungeon != 0)
    {
        if (dungeon->get_index() == 10000)
            allowPenalty = false;
    }

    if (a3)
    {
        OnSuccessedDungeonClear(user);
    }
    else
    {
        if (get_member_count() == 1)
        {
            CheckHackAverageHitCount();
            secu.dungeonFail();                         // +0x1abc
        }
        OnFailedDungeonClear(user);
    }

    if (user != 0)
    {
        if (bf.check_start_point())
        {
            WongWork::CHackAnalyzer* hackAnalyzer = user->getHackAnalyzer();
            hackAnalyzer->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x325,
                                           1, 0, 0);
        }
    }

    SaveCoin((eCoinSubReason)3);

    char eplpState = GetEPLPState();                    // -0x2d local_31
    if (eplpState == 1)
    {
        CUser* manager = getManager();
        if (manager->getMoveSpace() != (UserSpace::T)1)
        {
            int dungeonIdx = dungeon->get_index();
            GetInstanceDungeonEntranceLog()->DecrementDungeonEntrance(dungeonIdx,
                                                                      false);
        }
    }

    PacketGuard packet;                                 // -0x4c
    if (!m_autoCreated)                                 // +0x13c
    {
        if (!a3)
            set_charac_live(user, (ENUM_USER_DUNGEON_STATE)0);

        char memberNames[0x100];                        // -0x14c
        memset(memberNames, 0, sizeof(memberNames));
        _getMemberNames(memberNames);

        bool groupBranch;                               // (EPLP命令!=3 && 全灭)||成功||a4
        if (GetSelectedEPLPCmd() != 3)
        {
            if (check_allmember_die())
                groupBranch = true;
            else if (a3)
                groupBranch = true;
            else if (a4)
                groupBranch = true;
            else
                groupBranch = false;
        }
        else if (a3)
            groupBranch = true;
        else if (a4)
            groupBranch = true;
        else
            groupBranch = false;

        if (groupBranch)
        {
            for (int k = 0; k <= 3; ++k)                // -0x20
            {
                if (!_checkValidUser(k))
                    continue;
                if (dungeon != 0)
                {
                    int partyType = m_battleField.m_bloodState;
                    char* names = _getMemberNames(memberNames);
                    int diff = bf.get_dungeon_diff();
                    const char* dunName = dungeon->GetDungeonName();
                    m_member[k].m_pUser->m_historyLog.LeaveDungeon(
                        dunName, diff, names, partyType);
                }
                if (bf.m_villageMonsterId != 0)
                {
                    if (m_member[k].m_pUser != user)
                    {
                        if (a3)
                            OnSuccessedDungeonClear(m_member[k].m_pUser);
                        else
                            OnFailedDungeonClear(m_member[k].m_pUser);
                    }
                }
                GameWorld* gameWorld = G_GameWorld();
                gameWorld->out_from_dungeon(m_member[k].m_pUser);
                if (!pvpMode)
                {
                    if (!specialDungeon)
                    {
                        if (eplpState == 1)
                        {
                            if (allowPenalty)
                                m_member[k].m_pUser->giveup_panalty();
                        }
                    }
                }
            }

            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(0);
            packet.put_short(get_member_count());
            for (int n = 0; n <= 3; ++n)                // -0x1c
            {
                if (!_checkValidUser(n))
                    continue;
                m_member[n].m_pUser->make_basic_info((char*)&packet, 0);
            }
            packet.finalize(true);
            if (!IsExistInvisible())
                G_GameWorld()->send_all(packet);
            else
                send_to_party(packet);

            set_state(1);
            battle_data_reset();
        }
        else
        {
            int leaveResult = leave_user(user, (ENUM_PARTY_INFO_TYPE)2);  // -0x28
            if (leaveResult != 1)
            {
                G_GameWorld()->out_from_dungeon(user);
                if (!pvpMode)
                {
                    if (!specialDungeon)
                    {
                        if (eplpState == 1)
                        {
                            if (allowPenalty)
                                user->giveup_panalty();
                        }
                    }
                }

                packet.clear();
                packet.put_header(0, 2);
                packet.put_byte(0);
                packet.put_short(1);
                user->make_basic_info((char*)&packet, 0);
                packet.finalize(true);
                G_GameWorld()->send_all(packet, user);  // 排除放弃者的全服广播
            }
            sendInoutConditionDungeon(
                (RetryFailReason::T)checkInoutConditionDungeon());
        }
    }
    else                                                // 自动创建队伍：单人收尾
    {
        leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
        G_GameWorld()->out_from_dungeon(user);
        if (!pvpMode)
        {
            if (!specialDungeon)
            {
                if (eplpState == 1)
                {
                    if (allowPenalty)
                        user->giveup_panalty();
                }
            }
        }

        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(0);
        packet.put_short(1);
        user->make_basic_info((char*)&packet, 0);
        packet.finalize(true);
        if (getManager()->getMoveSpace() == (UserSpace::T)1)
            send_to_party(packet);
        else
            G_GameWorld()->send_all(packet, user);
    }

    CSpecialItemRoutingManager* routingMgr =
        G_CGameManager()->GetSpecialItemRoutingManager();
    if (routingMgr != 0)
        routingMgr->ProcessGiveUp(this);
