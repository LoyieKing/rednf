// ORIG 0x081c8102 T process（process.md 全文转写，含汇编分支次序）。
// §6.2 巨件核销：P167 95/95 [0缺0多]；gap 810→810/830；残差归因 N82/P112。
// 真修 1 条：死块 [HackUser] logFormat(0x198a) 误传 member 指针 → 按 ORIG
// [b28d-29] N82/P211：帧与参数副本槽（-0x90 vs -0x98）+ store 装载序，调用点全等  (ORIG 0x081c8102 insn 810->831)
// +0x13e 改 member->get_acc_id()（补齐缺失的 _ZNK5CUser10get_acc_idEv 调用点）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 840（ORIG 808 / ours 826），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
// [H5DisPatcher 核账] 2026-09-11 实测 ours 831→832（+1）劣化 ⇒ 已回滚留 N。
int DisPatcher_SelectDungeon::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
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

    // [G5-ProcB] ORIG 0x081c8110 param 槽 -0x90（函数头）→ 0x081c81a4
    // `mov 0x10(%ebp),%eax; mov %eax,-0x8c(%ebp)` msg 视图槽在 IsLightServer()
    // 之后才初始化。函数头先声明 req 会把槽基整体下移（我方 -0x9c/-0x98）。
    const MSG_SELECT_DUNGEON& req = *reinterpret_cast<const MSG_SELECT_DUNGEON*>(&msg);

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
                    // [K6-DpcProcess 2026-09-11] P59/P168a①：ORIG 为 `test %al,%al; je <循环尾>`
                    // （0x081c8d0a 段，2 条）；`if (!checkValidUser(i)) continue;` 负形使 4.4
                    // 产 `call; xor $0x1,%eax; test %al,%al; jne <体>`（3 条，每处 +1）。
                    // 改正向包裹复现 ORIG 直跳。
                    if (party->checkValidUser(i))
                    {
                        // P48：ORIG 本区两次独立 get_user(i)（首实参一次、getHackAnalyzer 一次）。
                        CUser* hackArg = party->get_user(i);
                        party->get_user(i)->getHackAnalyzer()->addServerHackCnt(
                            hackArg, (WongWork::ENUM_HACKTYPE)0x1f8, 1u, 0u, 0u);
                    }
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
        // [K6-DpcProcess 2026-09-11 第6轮] P0/P183：ORIG 0x081c8a6x 段为
        //   `call selectStage; mov %eax,0x4(%esp); mov 0xc(%ebp),%eax; mov %eax,(%esp);
        //    call procErrorSelectStage` —— 返回值**直传实参槽**、无具名槽；
        // 旧形 `const int selErr = …;` 多 store+load 两条（我方实测槽 -0x2c 往返）。
        // BLOCKED 符号见文档。
        advancealtar::Manager::procErrorSelectStage(
            user,
            (ENUM_ERROR)advancealtar::Manager::selectStage(
                user, (int)req.m_dungeonIdx,
                (advancealtar::StageDifficulty::T)(char)req.m_dungeonType));
        return 0;
    }

    if (dungeon->m_entryRestrictFlag != 0)
    {
        // ---- 等级下限检查型（ORIG 不判空 party，保持一致）----
        for (int i = 0; i < 4; ++i)
        {
            // [K6-DpcProcess 2026-09-11 第5轮] 同 0x081c8d0a 段模式：ORIG 直分支形
            // `test %al,%al; je <循环尾>`；`!` 负形多 1 条 `xor $0x1,%eax`（实测）。
            if (party->checkValidUser(i))
            {
                CUser* member = party->get_user(i);
                if (member != 0 && member->get_charac_level() < dungeon->get_min_level())
                {
                    out.m_c = 0xe;
                    return 0;
                }
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
                // [K6-DpcProcess 2026-09-11 第5轮] 同上（ORIG 直分支形）。
                if (party->checkValidUser(i))
                {
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
}
