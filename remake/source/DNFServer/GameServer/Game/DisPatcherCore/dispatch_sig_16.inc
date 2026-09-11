// ============================================================================
// ORIG T 0x081eb0c4 _ZN17DisPatcher_DieMob12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_DieMob/dispatch_sig.md，1564 insn）
// setMonsterMoveHack 门禁（moveCheckFlag && state==5）后按 state 分派：
//   5=普通副本 kill_monster 全反作弊链 / 10=死亡塔 handleKillMonster /
//   0xc=BossTower Global::CBossTowerFunc::DieMob / 0xd=advancealtar dieMob /
// [b28d-29] N78/N82：ORIG 无 0xa3f 字节 memset 而 4.4 发 call + 帧 0xb10 vs 0xb40；调用点全等  (ORIG 0x081eb0c4 insn 1564->1585)
//   8=WarRoom HandleMonsterKill。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 1582（ORIG 1564 / ours 1578），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [BigDispatchCore] 真语义缺口核销（实测 ndiff 824→759；P167 MISSING 8→2，仅剩变体）：
//  ① ORIG 0x081eb9b7/0x081ec396/0x081ec7a2/0x081ecac5/0x081ecb10 是逐层重新 call
//     get_state() 的 if-else 链（==5/0xa/0xc/0xd/8），原码折成 switch 少 4 次调用；
//  ② 4 处 `GetPartyIndex();`/`GetWarRoomIndex();` 丢弃直呼是我方凑数，ORIG
//     0x081eb635/0x081eb9d0/0x081ec629/0x081ecb29 各只 1 次（movswl 缓存），已删；
//  ③ ORIG 0x081ec355/0x081ec761 在 static ValidateFixMobDiePos 前确实 call
//     getHackAnalyzer 后丢弃返回值，已补。
// 残差封 N：1 处 memset(N78) + PacketGuard/std::pair/cMyTrace 的 C1↔C2、D1↔D2 变体。
int DisPatcher_DieMob::dispatch_sig(CUser* user, PacketBuf& packet)

{
    // [H5DisPatcher 核账 2026-09-11] 实测 ours 1617→1614 ✅（cntMap 引用 + 2 处 killWin 引用直算）
    MSG_MONSTER_DIE die;

    unsigned char passiveFlag = 0;               // ORIG -0x75（独立局部）
    unsigned char moveCheckFlag = 0;             // ORIG -0x76
    int monsterIdxOut = 0;                       // ORIG -0x74（kill_monster 出参）

    // [G5-SigA] P0/P183：ORIG 每个字段都是「具名基址 + add $0xNN」两步把 packed
    // 成员地址直接交给 get_*（0x081eb0d6 `lea -0xadb(%ebp),%eax; add $0xd,%eax`
    // = &die.m_mobSeq；0x081eb0e9 call get_short(unsigned short&)），全程无 tmp 槽
    // 往返。原实现的 5 个 tmp 中转每点多出「写 tmp + 读 tmp 回填字段」≈3 条，
    // 28 个调用点累计即 1644 vs 1564 的主要来源。此处以 cast 解除 packed 成员
    // 不可绑非 const 引用的限制后直传。
    // 原 memset(&die,...) 亦属发明代码：ORIG 全函数（0x081eb0c4..0x081ecce5）
    // 无 memset 符号调用、无 rep stos，die 仅按包逐个字段写入。

    if (!packet.get_short(*(unsigned short*)&die.m_mobSeq))
        return LineFunc(0x6d09,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 调用序列 t×8 后 s×7 ⇒
    // 0x6d0a/0x6d25/0x6d26 三处绑 `get_short(unsigned short&)`（ERt），原写法为 short&。
    if (!packet.get_short(*(unsigned short*)&die.m_fieldF))
        return LineFunc(0x6d0a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(*(int*)&die.m_field11))
        return LineFunc(0x6d0c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(*(int*)&die.m_field15))
        return LineFunc(0x6d0d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(*(unsigned int*)&die.m_killValue2))
        return LineFunc(0x6d0e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(*(unsigned int*)&die.m_killValue1))
        return LineFunc(0x6d0f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(*(unsigned char*)&die.m_mobCount))
        return LineFunc(0x6d11,
                        __PRETTY_FUNCTION__,
                        0, 0);
    for (int i = 0; i < (int)(unsigned char)die.m_mobCount; ++i)
    {
        if (!packet.get_short(*(unsigned short*)&die.m_skillList[i]))
            return LineFunc(0x6d16,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short(*(unsigned short*)&die.m_attackerList[i]))
            return LineFunc(0x6d17,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_int(*(unsigned int*)&die.m_damageList[i]))
            return LineFunc(0x6d18,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short(*(unsigned short*)&die.m_hitCountList[i]))
            return LineFunc(0x6d19,
                            __PRETTY_FUNCTION__,
                            0, 0);
    }
    if (!packet.get_short(*(unsigned short*)&die.m_killPairA24))
        return LineFunc(0x6d25,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(unsigned short*)&die.m_killPairA26))
        return LineFunc(0x6d26,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(*(unsigned char*)&die.m_fieldA22))
        return LineFunc(0x6d28,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(*(unsigned char*)&die.m_fieldA23))
        return LineFunc(0x6d2b,
                        __PRETTY_FUNCTION__,
                        0, 0);
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
    // [G5-SigA] ORIG 槽 -0x78（0x081eb68b `lea -0x78(%ebp),%eax`），无预清写
    // （全函数无 `movw $0x0,-0x78(%ebp)`）⇒ 去掉 `= 0`（P328⑤）。
    unsigned short unusedShort;                  // ORIG -0x78（读而未用）
    if (!packet.get_short(unusedShort))
        return LineFunc(0x6d3e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(*(char*)&die.m_fieldA28))
        return LineFunc(0x6d41,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(*(int*)&die.m_fieldA2d))
        return LineFunc(0x6d42,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(*(int*)&die.m_fieldA31))
        return LineFunc(0x6d43,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA29))
        return LineFunc(0x6d44,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA2b))
        return LineFunc(0x6d45,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA35))
        return LineFunc(0x6d46,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA37))
        return LineFunc(0x6d47,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA39))
        return LineFunc(0x6d48,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA3b))
        return LineFunc(0x6d49,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA3d))
        return LineFunc(0x6d4a,
                        __PRETTY_FUNCTION__,
                        0, 0);

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
        // [DPC_SigA] P0/P183：ORIG 0x081eba1e/ba4b/ba6d/baef **4 次**独立重算
        // `add/lea 0x1844(%eax)`(= &party->m_killMobCounter)，无引用缓存槽
        // （multidiff：我方独有 `mov %eax,<L>`+`mov <L>,%edx` 槽往返）⇒ 删引用。
        if (party->m_killMobCounter.size() <= 199)
        {
            std::map<int, int>::iterator it =
                party->m_killMobCounter.find((int)die.m_mobSeq);
            // [b28d-solo-DisPatcherCore] P167/P315：ORIG 为 `operator!=` +
            // `make_pair<unsigned short&,int>(m_mobSeq, 1)`（mangled IRtiE/OS3_OS6_），
            // 原写法 == / (int) cast 生成 eq + make_pair<int,int> 变体。
            if (it != party->m_killMobCounter.end())
                it->second += 1;
            else
            {
                unsigned short seq = die.m_mobSeq;   // packed 字段不可绑非 const 引用
                party->m_killMobCounter.insert(std::make_pair(seq, 1));
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
            // [K6-DpcSigBig L5] P0/P78：`return 0` 原位于 PacketGuard 作用域内 ⇒
            // 4.4 需把返回值跨析构保活 ⇒ 本函数全部 return 改用 ebx 载体（我方
            // 0x1633e `mov $0x0,%ebx; mov %ebx,%eax`；37 处 `return LineFunc(...)`
            // 各多 `mov %eax,%ebx`，合计 +37 条；实测 OUR mov$0,ebx=3 /
            // mov %eax,%ebx=37 vs ORIG 0 / 4）。ORIG 0x081ecca8 `jmp 081eccc8`
            // （析构段）→ 0x081eccd6 `mov $0x0,%eax`：**析构之后**才设返回值
            // （eax 载体）⇒ 原码该 return 不在析构对象作用域内。收紧作用域复现。
            {
                PacketGuard pgFail;
                pgFail.put_header(1, 0x2a);
                pgFail.put_byte(0);
                pgFail.put_byte(5);
                pgFail.put_short((int)die.m_mobSeq & 0xffff);
                pgFail.finalize(true);
                user->Send(pgFail);
            }
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
            // [DPC_SigA] P0/P183：ORIG 0x081ebca6 `add $0x8e448,%edx` 直算
            // (&user->m_mcap + 0x58) 后入实参，无引用缓存槽（Ghidra 该处为
            // `((st_monstorkill *)(param_1 + 0x8e448),...)` 直转型调用）。
            if (reinterpret_cast<CUser::st_monstorkill*>(
                    &user->m_mcap.m_monsterKillWindow[0])
                    ->Check(tick, die.m_killPairA24, die.m_killPairA26,
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
            // [K6-DpcSigBig L5] P0/P183：ORIG 0x081ebfa5..0x081ebfbc 该谓词
            // **无栈槽往返**（`call CParty::isMyMember` 后直接
            // `je 081ebfb5`(mov $0,%eax) / `mov $0x1,%eax` / `jmp` /
            // `test %al,%al; je 081ec2b3`，全程寄存器）⇒ 原码是内联表达式，
            // 非具名 bool 局部。我方旧形多 `mov %al,-0x3a(%ebp)` +
            // `movzbl -0x3a(%ebp),%eax`（槽往返 2 条）+ 一条 `test %eax,%eax`。
            // 德摩根等价改写（短路顺序不变：先判 attacker 非空）。
            if (attacker == 0 ||
                party->isMyMember((unsigned int)die.m_attackerList[i]) == 0)
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
            // [DPC_SigA] P0/P183：同上（ORIG 0x081ec3f5 `add $0x8e448,%edx`）。
            if (reinterpret_cast<CUser::st_monstorkill*>(
                    &user->m_mcap.m_monsterKillWindow[0])
                    ->Check(tick, die.m_killPairA24, die.m_killPairA26,
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
}
