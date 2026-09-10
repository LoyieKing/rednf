int DisPatcher_SelectDungeon::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const MSG_SELECT_DUNGEON& req = *reinterpret_cast<const MSG_SELECT_DUNGEON*>(&msg);
    stSelectDungeonParam& out = static_cast<stSelectDungeonParam&>(param);

    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x1970, __PRETTY_FUNCTION__, out.m_4, user->get_acc_id());
    }

    IsLightServer();   // ORIG 调用后丢弃结果

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0 && party->getManager() != user)
    {
        out.m_4 = 0x7fffffff;   // 非队长选副本
        return 0;
    }

    // ORIG +0x0fc~+0x1be（asm 权威，Ghidra C 未渲染该支）：msg[0x11]==1 ⇒ 全队
    // get_user(i≤3)→logFormat(0x198a "[HackUser]...")→leave_user(3)→DisConnSig(0x18,1,0)。
    if (static_cast<unsigned char>(req.m_field11) == 1)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* member = party->get_user(i);
            if (member != 0)
            {
                LogManager::logFormat(
                    1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__, 0x198a,
                    "[HackUser] dungeon hack user. (mid:%u)", member->get_acc_id());
                party->leave_user(member, (ENUM_PARTY_INFO_TYPE)3);
                member->DisConnSig((DISCONN_SIG)0x18, true, 0);
            }
        }
    }

    if (party != 0)
    {
        // CParty::m_battleField.m_bloodState(+0xcd8)（CParty.h public 区，具名访问）
        party->m_battleField.m_bloodState = (int)(char)req.m_field11;
    }

    if (user->isDisguiseCharac())
    {
        user->setDisguiseCharac(false, 0, 0);
    }

    if (party != 0)
    {
        const int memberCount = party->get_member_count();
        cMyTrace trace(__PRETTY_FUNCTION__, 0x19bf, 0);
        trace("[DisPatcher_SelectDungeon] partyCount : %d", memberCount);

        char hackFound = 0;
        const unsigned char selfVill = user->getCurCharacVill();
        const int selfArea = user->get_area(false);
        for (int i = 0; i < memberCount; ++i)
        {
            CUser* member = party->get_user(i);
            if (member == 0)
            {
                continue;
            }
            member->setDisguiseCharac(false, 0, 0);
            const unsigned char memberVill = member->getCurCharacVill();
            const int memberArea = member->get_area(false);
            if ((selfVill == memberVill && selfArea == memberArea) ||
                party->is_quick_party())
            {
                member->SetDeleteEventDungeonInvitation(false);
                continue;
            }
            LogManager::logFormat(
                1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__, 0x19df,
                "[HACK_VILL_AREA] m_id : %s, Vill(%d,%d), Area(%d,%d)",
                NumberToString(member->get_acc_id(), 0),
                (int)selfVill, (int)memberVill, selfArea, memberArea);
            hackFound = 1;
            break;
        }
        if (hackFound)
        {
            for (int i = 0; i < 4; ++i)
            {
                CUser* member = party->get_user(i);
                if (member == 0)
                {
                    continue;
                }
                LogManager::logFormat(
                    1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__, 0x19fa,
                    "[HACK_VILL_AREA] m_id : %s, Vill(%d), Area(%d)",
                    NumberToString(member->get_acc_id(), 0), (int)selfVill, selfArea);
                member->DisConnSig((DISCONN_SIG)0x18, true, 0);
            }
            out.m_4 = 0x7fffffff;
            return 0;
        }
    }

    bool checkSpecial = ((short)req.m_dungeonIdx != (short)10000);
    CDungeon* dungeon = G_CDataManager()->find_dungeon((int)req.m_dungeonIdx);
    if (dungeon != 0 && dungeon->m_advanceAltarSurvivalFlag != 0)
    {
        checkSpecial = false;
    }
    if (checkSpecial && party != 0)
    {
        const int worldIdx = G_GameWorld()->GetWorldMapIndex(user);
        if (worldIdx <= 0)
        {
            out.m_8 = 8;   // 不在有效世界图：错误 8，继续后续流程
        }
        else
        {
            CWorldMap* worldMap = G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
            if (worldMap == 0)
            {
                out.m_4 = 0x7fffffff;
                return 0;
            }
            if (!worldMap->hasDungeon(
                    static_cast<unsigned int>(static_cast<int>(req.m_dungeonIdx))))
            {
                for (int i = 0; i < 4; ++i)
                {
                    if (!party->checkValidUser(i))
                    {
                        continue;
                    }
                    // P48：ORIG 本区两次独立 get_user(i)（首实参一次、getHackAnalyzer 一次）。
                    CUser* hackArg = party->get_user(i);
                    party->get_user(i)->getHackAnalyzer()->addServerHackCnt(
                        hackArg, (WongWork::ENUM_HACKTYPE)0x1f8, 1u, 0u, 0u);
                }
                out.m_8 = 8;
                return 0;
            }
        }
    }

    char hasDeathFlag = 0;
    {
        const int worldIdx = G_GameWorld()->GetWorldMapIndex(user);
        if (worldIdx > 0)
        {
            CWorldMap* worldMap = G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
            if (worldMap != 0)
            {
                hasDeathFlag = worldMap->hasDeathTower() ? 1 : 0;
            }
        }
    }

    dungeon = G_CDataManager()->find_dungeon((int)req.m_dungeonIdx);
    if (dungeon == 0)
    {
        out.m_4 = 0x7fffffff;
        return 0;
    }

    // P48：ORIG 两处独立 getDungeonKind（==1 与 ==3 各一）。
    if (dungeon->getDungeonKind() == 1)
    {
        // ---- 死亡之塔 ----
        if (party == 0)
        {
            out.m_4 = 0x7fffffff;
            return 0;
        }
        WongWork::CDeathTower* tower = G_CGameManager()->getDeathTower();
        if (tower == 0)
        {
            out.m_4 = 0x7fffffff;
            return 0;
        }
        const int minLevel = dungeon->get_min_level();
        for (int i = 0; i < 4; ++i)
        {
            CUser* member = party->get_user(i);
            if (member != 0 && member->get_charac_level() < minLevel)
            {
                return 0;   // 有成员等级不足：静默忽略
            }
        }
        tower->attachParty(party);
        tower->handleStartGame((int)req.m_dungeonIdx, (char)req.m_dungeonType,
                               (ENUM_DUNGEON_TYPE)(char)req.m_field10);
        out.m_4 = 0x7fffffff;
        return 0;
    }
    if (dungeon->getDungeonKind() == 3)
    {
        // ---- BossTower（ORIG 此处不判空直接取成员数，保持一致）----
        const int memberCount = party->get_member_count();
        if ((signed char)dungeon->get_limit_party_count() < memberCount)
        {
            PacketGuard guard;
            guard.clear();
            guard.put_header(1, 0x10);
            guard.put_byte(0);
            guard.put_int(0xf3);
            guard.finalize(true);
            party->send_to_party(guard);
            return 0;
        }
        Global::CBossTowerFunc::SelectDungeon(party, (short)req.m_dungeonIdx,
                                              (ENUM_DUNGEON_TYPE)(char)req.m_field10);  // BLOCKED 符号见文档
        return 0;
    }

    if (dungeon->m_advanceAltarSurvivalFlag != 0)
    {
        // ---- 祭坛副本 ----
        const int selErr = advancealtar::Manager::selectStage(
            user, (int)req.m_dungeonIdx,
            (advancealtar::StageDifficulty::T)(char)req.m_dungeonType);   // BLOCKED 符号见文档
        advancealtar::Manager::procErrorSelectStage(
            user, (ENUM_ERROR)selErr);   // BLOCKED 符号见文档
        return 0;
    }

    if (dungeon->m_entryRestrictFlag != 0)
    {
        // ---- 等级下限检查型（ORIG 不判空 party，保持一致）----
        for (int i = 0; i < 4; ++i)
        {
            if (!party->checkValidUser(i))
            {
                continue;
            }
            CUser* member = party->get_user(i);
            if (member != 0 && member->get_charac_level() < dungeon->get_min_level())
            {
                out.m_c = 0xe;
                return 0;
            }
        }
    }
    else if (dungeon->m_villageAttackEventFlag != 0)
    {
        // ---- 复仇地下城 ----
        if ((char)req.m_dungeonType != 0)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x1f8, 1u, 0u, 0u);
            out.m_4 = 0x7fffffff;
            return 0;
        }
        if (party != 0)
        {
            const int memberCount = party->get_member_count();
            if (!GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon())
            {
                out.m_c = 0x15;
                return 0;
            }
            if ((signed char)dungeon->m_maxAttackPartyMember < memberCount ||
                memberCount < (signed char)dungeon->m_minAttackPartyMember)
            {
                out.m_c = 0xab;
                return 0;
            }
            for (int i = 0; i < 4; ++i)
            {
                if (!party->checkValidUser(i))
                {
                    continue;
                }
                CUser* member = party->get_user(i);
                const unsigned char revengeCnt = member->GetCurRevengeDungeonCount();
                if (revengeCnt == 0 || (unsigned char)dungeon->m_maxRevengeCount < revengeCnt)
                {
                    party->get_user(i)->SetCurRevengeDungeonCount(0);   // P48：ORIG 第二次独立 get_user
                    out.m_c = 0xad;
                    out.m_14 = i;
                    return 0;
                }
            }
        }
    }

    // ---- 公共开团路径（ORIG +0xc1f）----
    if (party != 0)
    {
        // ORIG 读 CParty::m_battleField.m_bloodState(+0xcd8) 字段与 1 比较（非 GetPartyType() 方法计算值）
        const bool skipFatigue = (hasDeathFlag != 0) ||
                                 (party->m_battleField.m_bloodState == 1) ||
                                 (checkBlueMarble(party) != 0) ||
                                 (dungeon->isNoFatigueDungeon() != 0);
        if (!skipFatigue)
        {
            const int fatigue = party->CheckMemberFatigue();
            if (fatigue > 0)
            {
                out.m_c = 0x16;
                out.m_8 = 0x16;
                out.m_14 = fatigue - 1;
                return 0;
            }
        }
        const int started = party->dungeon_start(
            (int)req.m_dungeonIdx, (char)req.m_dungeonType,
            (ENUM_DUNGEON_TYPE)(char)req.m_field10);
        if (started > 0)
        {
            out.m_c = started;
            out.m_8 = started;
        }
        else if (user->m_network.get_nat_type() == 0)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x326, 1u, 0u, 0u);
        }
    }
    return 0;
