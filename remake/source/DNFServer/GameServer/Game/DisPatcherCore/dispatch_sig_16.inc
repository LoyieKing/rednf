int DisPatcher_DieMob::dispatch_sig(CUser* user, PacketBuf& packet)
{
    MSG_MONSTER_DIE die;
    memset(&die, 0, sizeof(die));

    unsigned char passiveFlag = 0;               // ORIG -0x75（独立局部）
    unsigned char moveCheckFlag = 0;             // ORIG -0x76
    int monsterIdxOut = 0;                       // ORIG -0x74（kill_monster 出参）

    // packed 字段不可绑定非 const 引用，按 ORIG get_* 重载逐一读入临时量回填。
    unsigned short tmpU16 = 0;
    short tmpS16 = 0;
    int tmpS32 = 0;
    unsigned int tmpU32 = 0;
    unsigned char tmpU8 = 0;

    if (!packet.get_short(tmpU16))
        return LineFunc(0x6d09,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_mobSeq = tmpU16;
    // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 调用序列 t×8 后 s×7 ⇒
    // 0x6d0a/0x6d25/0x6d26 三处绑 `get_short(unsigned short&)`（ERt），原写法为 short&。
    if (!packet.get_short(tmpU16))
        return LineFunc(0x6d0a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldF = tmpU16;
    if (!packet.get_int(tmpS32))
        return LineFunc(0x6d0c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_field11 = tmpS32;
    if (!packet.get_int(tmpS32))
        return LineFunc(0x6d0d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_field15 = tmpS32;
    if (!packet.get_int(tmpU32))
        return LineFunc(0x6d0e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_killValue2 = tmpU32;
    if (!packet.get_int(tmpU32))
        return LineFunc(0x6d0f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_killValue1 = tmpU32;
    if (!packet.get_byte(tmpU8))
        return LineFunc(0x6d11,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_mobCount = tmpU8;
    for (int i = 0; i < (int)(unsigned char)die.m_mobCount; ++i)
    {
        if (!packet.get_short(tmpU16))
            return LineFunc(0x6d16,
                            __PRETTY_FUNCTION__,
                            0, 0);
        die.m_skillList[i] = tmpU16;
        if (!packet.get_short(tmpU16))
            return LineFunc(0x6d17,
                            __PRETTY_FUNCTION__,
                            0, 0);
        die.m_attackerList[i] = tmpU16;
        if (!packet.get_int(tmpU32))
            return LineFunc(0x6d18,
                            __PRETTY_FUNCTION__,
                            0, 0);
        die.m_damageList[i] = tmpU32;
        if (!packet.get_short(tmpU16))
            return LineFunc(0x6d19,
                            __PRETTY_FUNCTION__,
                            0, 0);
        die.m_hitCountList[i] = tmpU16;
    }
    if (!packet.get_short(tmpU16))
        return LineFunc(0x6d25,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_killPairA24 = tmpU16;
    if (!packet.get_short(tmpU16))
        return LineFunc(0x6d26,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_killPairA26 = tmpU16;
    if (!packet.get_byte(tmpU8))
        return LineFunc(0x6d28,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA22 = tmpU8;
    if (!packet.get_byte(tmpU8))
        return LineFunc(0x6d2b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA23 = tmpU8;
    if (!packet.get_byte(passiveFlag))
        return LineFunc(0x6d2f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(moveCheckFlag))
        return LineFunc(0x6d32,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (moveCheckFlag != 0 && user->get_state() == 5)
    {
        CParty* party =
            G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party == 0)
            return LineFunc(0x6d38,
                            __PRETTY_FUNCTION__,
                            0, 0);
        party->setMonsterMoveHack();
    }
    unsigned short unusedShort = 0;              // ORIG local_7c（读而未用）
    if (!packet.get_short(unusedShort))
        return LineFunc(0x6d3e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(die.m_fieldA28))
        return LineFunc(0x6d41,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(tmpS32))
        return LineFunc(0x6d42,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA2d = tmpS32;
    if (!packet.get_int(tmpS32))
        return LineFunc(0x6d43,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA31 = tmpS32;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d44,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA29 = tmpS16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d45,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA2b = tmpS16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d46,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA35 = tmpS16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d47,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA37 = tmpS16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d48,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA39 = tmpS16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d49,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA3b = tmpS16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d4a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA3d = tmpS16;

    // ORIG 不是 switch：0x081eb9b7(==5) / 0x081ec396(==0xa) / 0x081ec7a2(==0xc) /
    // 0x081ecac5(==0xd) / 0x081ecb10(==8) 是逐层重新 call get_state() 的 if-else 链
    // （每层 `cmp $k,%eax; sete %al; test; je next`）。原先折成 switch 少 4 次
    // get_state 调用，属真语义缺段。
    if (user->get_state() == 5)                 // ---- 普通副本击杀 ----
    {
        CParty* party =
            G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party == 0)
            return LineFunc(0x6d54,
                            __PRETTY_FUNCTION__,
                            0, 0);
        std::map<int, int>& cntMap = party->m_killMobCounter;
        if (cntMap.size() <= 199)
        {
            std::map<int, int>::iterator it =
                cntMap.find((int)die.m_mobSeq);
            // [b28d-solo-DisPatcherCore] P167/P315：ORIG 为 `operator!=` +
            // `make_pair<unsigned short&,int>(m_mobSeq, 1)`（mangled IRtiE/OS3_OS6_），
            // 原写法 == / (int) cast 生成 eq + make_pair<int,int> 变体。
            if (it != cntMap.end())
                it->second += 1;
            else
            {
                unsigned short seq = die.m_mobSeq;   // packed 字段不可绑非 const 引用
                cntMap.insert(std::make_pair(seq, 1));
            }
        }
        if (passiveFlag == 1)
        {
            party->DestroyPassiveObject((int)die.m_mobSeq);
            return 0;
        }
        int r = party->kill_monster(
            user, (int)die.m_mobSeq,
            static_cast<unsigned short>(die.m_fieldF), die, &monsterIdxOut);
        if (r == 0)
        {
            PacketGuard pgFail;
            pgFail.put_header(1, 0x2a);
            pgFail.put_byte(0);
            pgFail.put_byte(5);
            pgFail.put_short((int)die.m_mobSeq & 0xffff);
            pgFail.finalize(true);
            user->Send(pgFail);
            return 0;
        }

        // ---- kill_monster 成功后的过度伤害/位置反作弊链 ----
        static_cast<Secu_HackLogCheck*>(user->GetHackCheck())
            ->DieMob(&die);
        if ((unsigned short)die.m_fieldF == 0xffff)
        {
            user->m_mcap.m_monsterDieHackCnt += 1;
            if ((unsigned int)user->m_mcap.m_monsterDieHackCnt > 4)
            {
                user->m_mcap.m_monsterDieHackCnt = 0;
                // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 为 **3 个独立调用点**
                // （Ghidra C：dungeon==0 → (1,0,0) / index==0x2afe → (1,2,0) /
                // else → (1,0,0)），原三元折叠成 1 个（MISSING 0xcd×2 + getHackAnalyzer×2）。
                if (party->m_battleField.m_pDungeon == 0)
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACKTYPE)0xcd, 1, 0, 0);
                }
                else if (party->m_battleField.m_pDungeon->get_index() == 0x2afe)
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACKTYPE)0xcd, 1, 2, 0);
                }
                else
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACKTYPE)0xcd, 1, 0, 0);
                }
                if (!party->checkBossRoom())
                {
                    if (party->m_battleField.m_pDungeon == 0)
                    {
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0xce, 1, 0, 0);
                    }
                    else if (party->m_battleField.m_pDungeon->get_index() == 0x2afe)
                    {
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0xce, 1, 2, 0);
                    }
                    else
                    {
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0xce, 1, 0, 0);
                    }
                }
            }
        }
        else
        {
            unsigned int tick =
                GlobalData::s_systemTime_.getCurTickCount();
            CUser::st_monstorkill& killWin =
                *reinterpret_cast<CUser::st_monstorkill*>(
                    &user->m_mcap.m_monsterKillWindow[0]);
            if (killWin.Check(tick, die.m_killPairA24, die.m_killPairA26,
                              600, 600, 1000))
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0xd1, 1, 0, 0);
            }
            user->m_mcap.m_monsterDieHackCnt = 0;
        }
        for (int i = 0; i < (int)(unsigned char)die.m_mobCount; ++i)
        {
            if (die.m_skillList[i] != 0x111)
                continue;
            unsigned int limit = 0;
            WongWork::ENUM_HACKTYPE hackType =
                static_cast<WongWork::ENUM_HACKTYPE>(0x6b);
            CUser* attacker =
                G_GameWorld()->find_from_world(die.m_attackerList[i]);
            bool attackerOk =
                attacker != 0 &&
                party->isMyMember((unsigned int)die.m_attackerList[i]) != 0;
            if (!attackerOk)
                continue;
            CHackLog_OverDamage::GetDungeonLimitDamageAndHackType(
                attacker, limit, hackType);
            if (die.m_killValue1 > 0x5dc)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x6f, 1, 0, 0);
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x6ddb, 0);
                trace("Position : PacketDispatcher.cpp ZeroHit or 1500up Hit    Hit Count : %d  :::::::::::: ENUM_HACKTYPE_KILL_MONSTER_ZERO_HIT",
                      die.m_killValue1);
                die.m_killValue1 = 1;
            }
            bool zeroHitBossDg = false;
            if (die.m_killValue1 == 1)
            {
                zeroHitBossDg =
                    static_cast<CParty*>(attacker->GetParty())->m_battleField.m_pDungeon
                        ->get_index() == 0x2afe;
            }
            if (zeroHitBossDg)
            {
                attacker->getHackAnalyzer()->addServerHackCnt(
                    attacker, (WongWork::ENUM_HACKTYPE)0x7f, 1,
                    die.m_killValue1, 0);
            }
            if (die.m_killValue2 == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x6d, 1, 0, 0);
                die.m_killValue2 = 1;
            }
            if (die.m_hitCountList[i] == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x6e, 1, 0, 0);
                die.m_hitCountList[i] = 1;
            }
            unsigned int perHit =
                die.m_damageList[i] / (unsigned int)die.m_hitCountList[i];
            if (limit < perHit &&
                (party->m_battleField.m_pDungeon == 0 ||
                 party->m_battleField.m_pDungeon->get_index() != 0x2afe))
            {
                attacker->getHackAnalyzer()->addServerHackCnt(
                    attacker, hackType, 1, 0, 0);
                if (CHackLog_OverDamage::GetDungeonLimitMaxDamage() <
                    perHit)
                {
                    attacker->getHackAnalyzer()->addServerHackCnt(
                        attacker,
                        CHackLog_OverDamage::GetHackTypeOverLimitMaxDamage(),
                        1, 0, 0);
                }
            }
        }
        if (die.m_killValue1 % 10 == 0 && die.m_killValue2 % 10 == 0)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0xca, 1, 0, 0);
        }
        // ValidateFixMobDiePos 在 ORIG 是 static（0x080f908c，this 不入栈），但 ORIG
        // 源码仍写成 `user->getHackAnalyzer()->ValidateFixMobDiePos(...)`：0x081ec355
        // 处**先 call getHackAnalyzer 再丢弃返回值**。故此处保留直呼。
        user->getHackAnalyzer();
        WongWork::CHackAnalyzer::ValidateFixMobDiePos(
            user, (unsigned short)monsterIdxOut,
            die.m_killPairA24, die.m_killPairA26);
    }
    else if (user->get_state() == 10)            // ---- 死亡塔 ----
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower == 0)
            goto die_mob_end;                    // ORIG 0x081ec3bb je 公共出口
        if ((unsigned short)die.m_fieldF != 0xffff)
        {
            unsigned int tick =
                GlobalData::s_systemTime_.getCurTickCount();
            CUser::st_monstorkill& killWin =
                *reinterpret_cast<CUser::st_monstorkill*>(
                    &user->m_mcap.m_monsterKillWindow[0]);
            if (killWin.Check(tick, die.m_killPairA24, die.m_killPairA26,
                              600, 600, 1000))
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0xd4, 1, 0, 0);
            }
        }
        for (int i = 0; i < (int)(unsigned char)die.m_mobCount; ++i)
        {
            if (die.m_skillList[i] != 0x111)
                continue;
            CUser* attacker =
                G_GameWorld()->find_from_world(die.m_attackerList[i]);
            if (attacker == 0)
                continue;
            if (die.m_hitCountList[i] == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x6e, 1, 0, 0);
                die.m_hitCountList[i] = 1;
            }
            unsigned int perHit =
                die.m_damageList[i] / (unsigned int)die.m_hitCountList[i];
            if (perHit > 12999)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x6c, 1, 0, 0);
            }
            if (CHackLog_OverDamage::GetDeathTowerLimitDamage() < perHit)
            {
                attacker->getHackAnalyzer()->addServerHackCnt(
                    attacker,
                    CHackLog_OverDamage::GetDeathTowerOverDamageHackType(),
                    1, 0, 0);
            }
        }
        if ((unsigned short)die.m_fieldF == 0xffff)
        {
            CParty* party =
                G_CGameManager()->GetParty(user->GetPartyIndex());
            if (party == 0)
                return LineFunc(0x6e7f,
                                __PRETTY_FUNCTION__,
                                0, 0);
            user->m_mcap.m_monsterDieHackCnt += 1;
            if ((unsigned int)user->m_mcap.m_monsterDieHackCnt > 4)
            {
                user->m_mcap.m_monsterDieHackCnt = 0;
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0xcd, 1, 1, 0);
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0xce, 1, 1, 0);
            }
        }
        else
        {
            user->m_mcap.m_monsterDieHackCnt = 0;
        }
        tower->handleKillMonster(
            user, (unsigned short)die.m_mobSeq,
            (unsigned short)die.m_fieldF, die, &monsterIdxOut);
        user->getHackAnalyzer();                 // ORIG 0x081ec761（同上，直呼丢弃）
        WongWork::CHackAnalyzer::ValidateFixMobDiePos(
            user, (unsigned short)monsterIdxOut,
            die.m_killPairA24, die.m_killPairA26);
    }
    else if (user->get_state() == 12)            // ---- BossTower ----
    {
        if ((unsigned short)die.m_fieldF == 0xffff)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0xcd, 1, 0, 0);
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0xce, 1, 0, 0);
        }
        for (int i = 0; i < (int)(unsigned char)die.m_mobCount; ++i)
        {
            if (die.m_skillList[i] != 0x111)
                continue;
            CUser* attacker =
                G_GameWorld()->find_from_world(die.m_attackerList[i]);
            if (attacker == 0)
                continue;
            unsigned int perHit =
                die.m_damageList[i] / (unsigned int)die.m_hitCountList[i];
            if (CHackLog_OverDamage::GetBossTowerLimitDamage() < perHit)
            {
                attacker->getHackAnalyzer()->addServerHackCnt(
                    attacker,
                    CHackLog_OverDamage::GetBossTowerOverDamageHackType(),
                    1, 0, 0);
                if (CHackLog_OverDamage::GetDungeonLimitMaxDamage() <
                    perHit)
                {
                    attacker->getHackAnalyzer()->addServerHackCnt(
                        attacker,
                        CHackLog_OverDamage::GetHackTypeOverLimitMaxDamage(),
                        1, 0, 0);
                }
            }
            if (die.m_killValue1 > 0x5dc)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x6f, 1, 0, 0);
                die.m_killValue1 = 1;
            }
            if (die.m_hitCountList[i] == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x6e, 1, 0, 0);
                die.m_hitCountList[i] = 1;
            }
            if (die.m_killValue2 == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x6d, 1, 0, 0);
                die.m_killValue2 = 1;
            }
        }
        Global::CBossTowerFunc::DieMob(
            user, (unsigned short)die.m_mobSeq,
            (unsigned short)die.m_fieldF, die, 0);
    }
    else if (user->get_state() == 13)            // ---- advancealtar ----
    {
        advancealtar::Manager::dieMob(
            user, (unsigned short)die.m_mobSeq,
            (unsigned short)die.m_fieldF, die);
    }
    else if (user->get_state() == 8)             // ---- 战争房间 ----
    {
        WarRoom* war =
            G_CGameManager()->GetWarRoom(user->GetWarRoomIndex());
        if (war == 0)
            return LineFunc(0x6ed0,
                            __PRETTY_FUNCTION__,
                            0, 0);
        CUser* killer = 0;
        if ((unsigned short)die.m_fieldF != 0xffff)
        {
            killer =
                G_GameWorld()->find_from_world(die.m_fieldF);
            if (killer == 0)
                return LineFunc(0x6edb,
                                __PRETTY_FUNCTION__,
                                0, 0);
        }
        if (!war->HandleMonsterKill(user, (int)die.m_mobSeq, killer))
        {
            PacketGuard pgFail;
            pgFail.put_header(1, 0x2a);
            pgFail.put_byte(0);
            pgFail.put_byte(5);
            pgFail.put_short((int)die.m_mobSeq & 0xffff);
            pgFail.finalize(true);
            user->Send(pgFail);
        }
    }
die_mob_end:
    return 0;
