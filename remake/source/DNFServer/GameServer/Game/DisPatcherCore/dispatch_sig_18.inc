int DisPatcher_SetPlayResult::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 5)
        return 0;

    // ORIG +0x31..+0x45：GetPartyIndex 单次调用（movswl %ax,%ebx 缓存），勿重复。
    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
        return LineFunc(0x8155,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (party->m_battleField.m_bloodState == 1)
        return 0;
    // ORIG +0x99..+0xc4（前批缺段）：party 非空且 party+0xcac(dungeon) 非空且
    // dungeon+0x89f(m_bloodDungeonType) 有符号 >0 → 直接返 0。dungeon 独立具名槽
    // （ORIG -0x3c(%ebp)，技13）。
    if (party != 0)
    {
        CDungeon* dungeon = party->m_battleField.m_pDungeon;      // ORIG -0x3c(%ebp)
        if (dungeon != 0)
        {
            if ((char)dungeon->m_bloodDungeonType > 0)   // ORIG `test %al,%al; jle`
                return 0;
        }
    }

    // ---- 结果区：party+0x17f4 为 GameResultType（CParty.h 现为权威成员
    //      GameResultSet m_gameResultSet @+0x17f4，GameResultSet : GameResultType
    //      且零增成员 ⇒ 布局/偏移不变，直接取其址，无视图 cast）；
    //      字段偏移/宽度证据 docs/class_func_reports/DisPatcher_SetPlayResult/
    //      dispatch_sig.md：0x17fd→m_field9 / 0x17fe→m_fielda / 0x183e→m_field4a /
    //      0x1808+8i→m_shot[i].m_count / 0x180c+8i→m_shot[i].m_field4 /
    //      0x1840→m_field4c / 0x17f8→m_field4 / 0x17fc→m_field8）----
    GameResultType* gameResult = &party->m_gameResultSet;   // +0x17f4
    gameResult->Clear();

    if (!packet.get_byte(*reinterpret_cast<char*>(&gameResult->m_field9)))  // +0x17fd
        return LineFunc(0x816d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(&gameResult->m_fielda))) // +0x17fe
        return LineFunc(0x8172,
                        __PRETTY_FUNCTION__,
                        0, 0);
    unsigned short resultIdx = 0;
    // ORIG 0x081f4e5f `call _ZN9PacketBuf9get_shortERt`（unsigned short&）——
    // 原写法用 `*reinterpret_cast<short*>` 会绑到 ERs 重载，与 ORIG 符号不符，
    // 现按权威字段类型 m_field4a(ushort) 直传。
    if (!packet.get_short(gameResult->m_field4a))   // +0x183e
        return LineFunc(0x8174,
                        __PRETTY_FUNCTION__,
                        0, 0);

    char memberCountByte = 0;                    // ORIG local_ea3
    if (!packet.get_byte(memberCountByte))
        return LineFunc(0x8179,
                        __PRETTY_FUNCTION__,
                        0, 0);
    for (int i = 0; i < memberCountByte && i < 4; ++i)
    {
        unsigned short memberVal = 0;
        int memberCoin = 0;
        // 每成员结果二元组 = GameResultType::m_shot[i]{m_count(ushort),m_field4(int)}
        // （m_shot@+0x14、stride 8 → party+0x1808/0x180c；ORIG 0x081f4eec 以
        //  (i+0x300)*8+8 的「全局 8B 槽号」形态算址，此处按真实成员布局表达，
        //  P30 结构性寻址伪影差异已标注）
        if (!packet.get_short(gameResult->m_shot[i].m_count))
            return LineFunc(0x817e,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_int(gameResult->m_shot[i].m_field4))
            return LineFunc(0x8181,
                            __PRETTY_FUNCTION__,
                            0, 0);
        (void)memberVal;
        (void)memberCoin;
    }
    user->m_historyLog.MapClear(
        gameResult->m_field4a,
        static_cast<int>(party->GetMapPlayingTime() / 1000), 0);

    int killCountRead = 0;              // ORIG local_ea8（P167：ORIG get_int(int&) ERi）
    if (!packet.get_int(killCountRead))
        return LineFunc(0x818b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // ORIG +0x375：party 外层重判（cmpl $0,-0x48; je 尾）；&& 条件物化双常量岛
    // （`!check()` xor 形先行、`kill!=0` 后行 → mov$1/mov$0 岛 + test al; je）。
    if (party != 0)
    {
        // [b28d-29] P0 清绑：原 `register bool bossCheat asm("al")` 违 P0/N96，已删；
        // ORIG 短路物化岛（mov $1/$0 + test）4.4 恒折直分支 ⇒ 形态差记 N（setcc-fold）。
        bool bossCheat = !party->checkBossRoom() && killCountRead != 0;
        if (bossCheat)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x1fb,
                static_cast<unsigned int>(killCountRead), 0, 0);
        }
    }
    // +0x1840 = GameResultType::m_field4c（头内声明 int；ORIG 0x081f50dd 以
    // get_int(unsigned int&) ERj 装载，故保留同宽度无符号引用视图）
    unsigned int* coinSlot =
        reinterpret_cast<unsigned int*>(&gameResult->m_field4c);
    if (!packet.get_int(*coinSlot))
        return LineFunc(0x8195,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (*coinSlot > 10000)
    {
        // ORIG 0x081f5134/0x081f515b 两次独立读 party+0x1840，分别算 over(-0x34)
        // 与 rest(-0x30)，二者均在 addServerHackCnt 之前算好（P0 具名局部）。
        unsigned int over = *coinSlot / 10000;
        unsigned int rest = *coinSlot % 10000;
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x78, 1, over, 0);
        *coinSlot = rest;
    }

    // ORIG 本件 _ZNK15CUserCharacInfo14get_charac_jobEv 计数 = 2（我方原存 `int job`
    // 局部复用只 1 次）：与 DisPatcher_MoveMap::CheckOneMapPlayTimeHackCnt
    // 0x081c4f83/0x081c4f93 同型 —— 原码两处判定各自重新 call（getter 无副作用，
    // 展开后可观察行为完全不变）。
    // ORIG 0x081f5202/0x081f526b：if/else 两分支各自完整内联
    // fildll/fldl/fmulp/fistpll（真分支 _DAT_08bd6320=0.5、假分支
    // _DAT_08bd6328=0.8）⇒ 原码无 ratioDelta 局部（P0）；疲劳特化
    // job==3 或 job==4 且 growtype==2 → ×0.5。
    if (reinterpret_cast<CUserCharacInfo*>(user)->get_charac_job() == 3 ||
        (reinterpret_cast<CUserCharacInfo*>(user)->get_charac_job() == 4 &&
         reinterpret_cast<CUserCharacInfo*>(user)
             ->getCurCharFirstGrowType() == 2))
    {
        *coinSlot = static_cast<unsigned int>(
            static_cast<long long>(0.5 * static_cast<double>(*coinSlot)));
    }
    else
    {
        *coinSlot = static_cast<unsigned int>(
            static_cast<long long>(0.8 * static_cast<double>(*coinSlot)));
    }
    if (party->GetMapPlayingTime() / 1000 < *coinSlot)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x75, 1, *coinSlot, 0);
    }

    CBattle_Field& field =
        *reinterpret_cast<CBattle_Field*>(&party->m_battleField);
    gameResult->m_field4 = field.get_dungeon_index();          // +0x17f8
    gameResult->m_field8 =
        static_cast<unsigned char>(field.get_dungeon_diff());  // +0x17fc
    party->IncreaseSetPlayResultCount(user);

    if (party->get_dungeon_clear_state() == 0)
    {
        if (!field.IsClearDungeon())
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x196, 1, 0, 0);
            return 0;
        }
        int key = party->gen_timer_key((TIMER_MESSAGE)0x29);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                    party->GetPartyIndex(),
                                    (TIMER_MESSAGE)0x29, 0xf, key, 0);
        party->set_dungeon_clear_state(1);
    }
    // ORIG _ZN13CBattle_Field17GetCurrentMapInfoEv 计数 = 2：第一个判定直呼，
    // 之后才取具名局部供后续两处复用（同族"ORIG 不缓存"形）。
    if (field.GetCurrentMapInfo()->m_killChecker1.m_useSkillMaterialTime == 0)  // +0x4c
    {
        SIG_LOAD_CHARAC sigCharac;               // ORIG local_15bf 清零缓冲
        memset(&sigCharac, 0, sizeof(sigCharac));
        // ORIG 0x081f5601 `test %al,%al; je`（返回值门禁）+ 0x081f5605
        // `movzwl -0x158a(%ebp),%eax`：-0x158a = 缓冲基址 -0x15bb + 0x31
        // = SIG_LOAD_CHARAC::m_characData(+0xc)::m_lev(+0x25)（pack(1)）。
        if (user->GetCharacInfo(&sigCharac) &&
            0x1d < sigCharac.m_characData.m_lev)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0xe2, 1, 0, 0);
        }
    }
    MapInfo* mapInfo = field.GetCurrentMapInfo();
    if (mapInfo->m_killChecker1.m_useSkillMaterialTime != 0)
    {
        int useSkillMaterialCount =
            mapInfo->m_killChecker1.getUseSkillMaterialCount();
        unsigned int playSec = party->GetMapPlayingTime() / 1000;
        unsigned int tickUnits = playSec / 10;
        if (playSec % 10 != 0)
            ++tickUnits;
        // ORIG 0x081f56df `mov -0x44(%ebp),%eax; cmp -0x28(%ebp),%eax; jae`：
        // int 与 unsigned tickUnits 的**无符号**比较（int→unsigned 隐式提升）。
        // 原 `static_cast<int>(tickUnits)` 会生成有符号 jle，count 为负时行为不同。
        if (static_cast<unsigned int>(useSkillMaterialCount) < tickUnits)
        {
            SIG_LOAD_CHARAC sigChk;
            memset(&sigChk, 0, sizeof(sigChk));
            // ORIG 0x081f583b `test %al,%al; je` + 0x081f583f
            // `movzwl -0x1c8d(%ebp),%eax`：-0x1c8d = 缓冲基址 -0x1cbe + 0x31
            // = 同上 m_characData.m_lev。
            if (user->GetCharacInfo(&sigChk) &&
                0x1d < sigChk.m_characData.m_lev)
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0xe3, 1, 0, 0);
        }
    }

    unsigned int playMs = party->GetMapPlayingTime();     // ORIG local_44
    if (playMs < 10000 &&
        10 < reinterpret_cast<CUserCharacInfo*>(user)->get_charac_level())
    {
        int fastCnt = 11 - playMs / 1000;                 // ORIG local_28
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0xe5,
            static_cast<unsigned int>(fastCnt), 0, 0);
    }
    if (playMs < 20000 &&
        10 < reinterpret_cast<CUserCharacInfo*>(user)->get_charac_level())
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0xe4, 1, 0, 0);
    }

    int memberKillSum = 0;               // local_ebc（P167：ORIG get_int(int&)）
    int coinUsedA = 0;                   // local_eac
    int coinDenomA = 0;                  // local_eb4
    int expGain = 0;                     // local_eb0
    int expDenomB = 0;                   // local_eb8
    if (!packet.get_int(memberKillSum))
        return LineFunc(0x820e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(coinUsedA))
        return LineFunc(0x820f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(expGain))
        return LineFunc(0x8210,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(coinDenomA))
        return LineFunc(0x8211,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(expDenomB))
        return LineFunc(0x8212,
                        __PRETTY_FUNCTION__,
                        0, 0);

    bool tailChecksOk =
        !(memberKillSum != 0 || party->GetUsedCoinCount() != 0 ||
          field.GetTotalSkillMaterial() != 0 ||
          coinDenomA == 0 ||
          100.0f * (static_cast<float>(coinUsedA) /
                    static_cast<float>(coinDenomA)) < 99.98f ||
          expDenomB == 0 ||
          100.0f * (static_cast<float>(expGain) /
                    static_cast<float>(expDenomB)) < 99.98f);
    if (tailChecksOk)
    {
        bool pillFound = false;                           // ORIG local_21
        for (int i = 0; i < 4; ++i)
        {
            if (!party->checkValidUser(i))
                continue;
            // ORIG 0x8231a9/0x8232f6：GetInvenSlot(0, 0xc) 与 GetInvenSlot(0, 0xe)
            // （invenType=0，槽 0xc/0xe）；NEW 原为 (1,0xb)/(1,0x9) 属转写错误。
            // ORIG 里 _ZN6CParty9GetMemberEv / _ZNK15CUserCharacInfo18getCurCharacInvenREv
            // 计数各为 2 ⇒ 两个槽各自重新取 inventory（原码不缓存 memberInv）。
            Inven_Item slotItem =
                party->GetMember()[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 0xc);
            if (slotItem.m_addInfo == 0x2ad4)
            {
                pillFound = true;
                break;
            }
            Inven_Item slotItem2 =
                party->GetMember()[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 0xe);
            if (slotItem2.m_addInfo == 0x329e)
            {
                pillFound = true;
                break;
            }
        }
        if (!pillFound)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x1a2, 1, 0, 0);
        }
    }
    if (!party->CheckRecvFromAllMember(user))
        return 0;
    if (!party->SetPlayResult(user))
        return LineFunc(0x8235,
                        __PRETTY_FUNCTION__,
                        0, 0);
    return 0;
