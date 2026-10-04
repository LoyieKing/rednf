// [round5-A20 复核·N82/P98] fresh .o 2324/2296（gap +28）、判据⑪ frame ORIG 0x15c vs
//   我方 0x1bc（+0x60 = ~24 个 4B 槽）：ORIG 把 `data` 形参副本按需惰性落槽
//   （`mov 0x10(%ebp),%eax; mov %eax,-0x60(%ebp)`），我方前移至 -0xc4 序言；
//   另有 1 处 ORIG 无的 `lea -0xe4; mov (%esp); call` ⇒ 4.1/4.4 槽分区与重载选型差
//   （N82 附/N98），语义完成、p167 CLEAR。
int Inter_LoadEtc::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [BigInterLoadEtc 2026-09-09] ORIG 2296 / 我方 2325（gap +29，起点 +74）；
    // p167 CLEAR（436/436，差集仅 C1↔C2/D1↔D2 镜像）；TU 四数 65/99/0/12 全程未退。
    // 本轮三件**真缺口修复**（非形态）：
    //  ① getDungeonMinimumRequiredLevel/isDungeonOpen 实参 -1 → 0x2aff
    //     （ORIG 0x084c1279/0x084c1297 `movl $0x2aff,0x4(%esp)`）；
    //  ② peerIp 缓冲 16B→0x20（ORIG local_b4 跨 -0xb0..-0x90，if 分支用 +0x10 段，
    //     原声明越界写 16B）+ else 分支补 16B 清零（ORIG 4×`movl $0x0,disp`）；
    //  ③ getInitSkillTreeGrade 实参 myGrade → initBase
    //     （ORIG 0x084c09e6 `movzbl %al,%esi; mov %esi,0x4(%esp)`）。
    // 形态收敛（P259/P52/P0/P328）：Acquire 内联 ×6、set_skill_slot/skillW 链式、
    // 具名 bool ×5 去槽、premium/CAchievement/autoMarket/crane/hero/ev/itemLock/
    // titleBook 链式、nowSec/nowTick/lastPlay 内联、4 处错误码检查补函数级
    // `bool ok`（ORIG 共用 -0x61 槽）、insert_game_world 具名 uchar（P3 收窄）。
    // 残差 = N82 帧/槽复用（4.1 不复用、4.4 复用 ⇒ 槽号错位）+ N86 迭代器临时槽
    // + P292/N122 类 bool 归一化跨版本差。
    SIG_LOAD_ETC* sig = reinterpret_cast<SIG_LOAD_ETC*>(data);

    if (user->get_state() == 0)
        return 0xa7b;
    if (user->getCurCharacR() == 0)
        return 0xa7f;
    if (sig->m_field8 == 1)
        return 0xa96;

    PacketGuard packet;
    {
        // [BigUserLogic] ORIG 0x084c02??：无 grade 局部（两处各自重取 sig+0x354，
        // logFormat 实参为 43-44 的 mov -0x60;mov 0x354）。原 grade 槽产 store+load。
        if (!G_CDataManager()->IsAvailablePvPChannel(sig->m_pvpResult.m_pvpGrade))
        {
            LogManager::logFormat(1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__, 0xa9b,
                "Inter_LoadEtc::dispatch_sig pvp_grade = %d", sig->m_pvpResult.m_pvpGrade);
            return 0xa9c;
        }
    }

    // ---- 失败链前置处理（ORIG 各 err 分支统一收口于 LAB_084c2653）----
    // [BigUserLogic] 原 do-while+bccBranch/c14Branch 标志形是我方重写产物：ORIG
    // 各失败点为 `mov $code,%ebx; jmp <统一尾>` = 源码直接 return（编译器自动落
    // 析构尾）。改回 return 形，消除 4 个物化 bool 及其清零/读回指令。
    bool pvpTreeSkip = false;
    {
        // [BigInterLoadEtc] ORIG 0x084c0352..0x084c0472 四处检查共用同一具名
        // bool 槽 -0x61（`mov %al,-0x61; movzbl -0x61,%eax; xor $1; test; je`）
        // ⇒ 原码是函数级 `bool ok;` 各分支赋值复用（P328⑪/P320），非内联 !call()。
        bool ok;
        ok = SetPvpRecord(user, sig);
        if (!ok) return 0xaa2;
        ok = SetRankRecord(user, sig);
        if (!ok) return 0xaa5;

        // [BigUserLogic] ORIG 0x084c03xx：先物化 getCurCharacR()->+0x7b 与 sig+4
        // 两实参，**再**构造 cMyTrace，后调 operator()。原写法把实参求值放到
        // ctor 之后，指令序整体错位。
        long playTick = user->getCurCharacR()->m_lastPlayTick;
        unsigned charNo = (unsigned)sig->m_characNo;
        cMyTrace trace(__PRETTY_FUNCTION__, 0xac5, 0);
        trace("CharLastPlayTick char(%d) t(%d)", charNo, (unsigned)playTick);

        user->ProcPremiumFatigue();
        // [BigUserLogic] ORIG 0x084c040e 无判空直调。
        // [BigInterLoadEtc] ORIG O106-O110 无 premium 具名槽（eax 直传 this）。
        user->GetPremiumInfo()->RecalcAdditionalInfo(user);

        ok = SetFatigue(user);
        if (!ok) return 0xacf;
        verifyCharacVillage(user);
        ok = SendResult(user);
        if (!ok) return 0xae6;

        user->SendRestrictedGoods(false, 0);
        // §4.3 具名化：+0x2308 区在 SigTypes_C.h:464 已有 union 具名叠加
        // （m_cooltimeItem @+0x230c = m_pad2308+4、m_effectItem @+0x24a4 =
        //  m_pad2308+0x19c；写点证据 CUserCharacInfo::GetContinuousItemInfo
        //  0x08677f24 / DB_LoadEtc LoadCharacItemStat.md cap 0x194）。
        user->LoadCoolTimeItemInfo(
            &sig->m_contItemCooltime.m_info);
        user->LoadEffectItemInfo(
            &sig->m_contItemEffect.m_info);

        typedef charac_expand::CData CDataAlias;
        {
            // [BigUserLogic] ORIG 0x084c04c7+：GetCharacExpandData 结果留 %eax
            // 直发虚调用（不 spill）。命名变量形会让 4.4 多 2 条槽往返，改嵌套单行。
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(6)))->loadData(user, (char*)&sig->m_redeemInfo);
            // [hc 收] ORIG 0x08660ec4 区：sig->m_mixedRegion 为 SIG_LOAD_ETC 载荷
            // 线格式缓冲（§5.3 序列化缓冲豁免），+0x415 为该 expand-data 的固定布局
            // 偏移；loadData(user, ptr) 取地址直传，偏移为承载格式固有，seek 无漏。
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(8)))->loadData(user, sig->m_mixedRegion + 0x415);
            // [hc 收] +0xd58 同承载体 0xd 型 expand-data 布局偏移（§5.3 豁免）。
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xd)))->loadData(user, sig->m_mixedRegion + 0xd58);
        }

        int tagIdx = user->get_charac_idx(sig->m_fieldC);
        // [BigInterLoadEtc] ORIG 0x084c05ae..0x084c05b4：末条件独立
        // `test %eax,%eax; setne %al; test %al,%al; je`（P292 非 bool 返回直测），
        // 合入 && 链会被 4.4 折成 mov $1/jmp/mov $0 物化 ⇒ 拆嵌套 if。
        if (sig->m_fieldC != 0 && tagIdx != -1)
        {
            if (user->getTagCharacR() != 0)
            {
            // [BigUserLogic] ORIG 0x084c09??：先物化 sig+0x1a8/sig+0x10 两实参，
            // 再调 getTagCharacSkillW，最后一并传 set_skill_slot。原写法在 call 后
            // 又经 tagSkillBuf/skillW 两跳槽写入（多 2 条 store + 求值序颠倒）。
            // [BigInterLoadEtc/P52] 三具名局部各多 2 条槽往返（ORIG 用 %esi/%ebx/%eax
            // 保活，无槽）⇒ 合并为单条链式表达式，让 4.4 复用 callee-saved 寄存器。
            reinterpret_cast<SkillSlot*>(user->getTagCharacSkillW())->set_skill_slot(
                (char*)sig->m_tagEtc.m_skills[0],    // sig+0x10（行 0 = tagSkillSlot）
                (char*)sig->m_tagEtc.m_skills[1],    // sig+0x1a8（行 1 = tagSkillCombo）
                0x198);
            }
        }
        if (tagIdx == -1)
            sig->m_mixedRegion[0xd3b] = 0;

        {
            // [hc 收] +0xbe7 / +0xd3c 同承载体 0xb/0xa 型 expand-data 布局偏移
            // （§5.3 线格式序列化缓冲豁免，ORIG loadData 实参即固定偏移）。
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xb)))->loadData(user, sig->m_mixedRegion + 0xbe7);
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(10)))->loadData(user, sig->m_mixedRegion + 0xd3c);
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0x10)))->loadData(user, sig->m_mixedRegion + 0xd7c);
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xe)))->loadData(user, (char*)&sig->m_titlebook);
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xf)))->loadData(user, (char*)&sig->m_achievement);
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0x11)))->loadData(user, sig->m_mixedRegion + 0x2308);
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0x12)))->loadData(user, sig->m_mixedRegion + 0x2310);
            // [BigUserLogic] ORIG 0x084c09?? 单条 `add $0x14a9a,%edx`：实参直接用
            // sig 基址+常量（m_attendanceEvent(+0x14a99)+1 的写法产两条 add）。
            static_cast<CDataAlias*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0x13)))->loadData(
                user, reinterpret_cast<char*>(sig->m_attendanceEvent.m_pad));
        }

        // §4.3 具名化：m_expandDataF(+0xb1a4)+0x1804 = +0xc9a8 =
        // SigTypes_C.h:546 具名 m_achievementLastUpdate（DB_LoadEtc
        // LoadAchievement.md：+0xb1a4 解压 0x1800 数据、+0xc9a4 计数、
        // +0xc9a8 last_update get_uint → +0xc9ac 衔接具名字段）。
        if (sig->m_achievement.m_saveTime == 0)
        {
            // [BigInterLoadEtc] ORIG 0x084c0530..0x084c0538：结果留 %eax 直发
            // this（无具名槽）⇒ 链式单语句。
            reinterpret_cast<CAchievement*>(user->GetCharacExpandData(
                ENUM_CHARAC_EXPAND_TYPE(0xf)))->adjust();
        }

        // ---- 登录历史（CNetwork 视图位于 CUser+0xe0）----
        // ORIG:get_ip()==0 与否则两分支（仅 peerIp 缓冲写法不同，实参序一致），
        // 每分支独立调用 GetServerGroup/GetLogInOutState/G_CEnvironment/get_port/
        // GetPeerIP2/getWebAddress + Login（无缓存局部）。
        {
            CNetwork<4096, 450000>* net = &user->m_network;
            // [BigInterLoadEtc 修复] ORIG local_b4 跨 -0xb0..-0x90（0x20 字节）：
            // if 分支用 +0x10 段（lea -0xa0），else 分支用基址段（lea -0xb0）；
            // 原声明 char[16] 令 if 分支 GetPeerIP2(peerIp+0x10,0x10) 越界写 16B。
            // 声明为 uint[8] 使 else 清零复现 ORIG 的 4×`movl $0x0,disp(%ebp)` 单条形。
            unsigned int peerIp[8];   // [b28d-63] ORIG 0x84c0a23 memset 仅 m_mixedRegion（0x414）
            if (net->get_ip() == 0)
            {
                int serverGroup = (int)user->GetServerGroup();
                // [BigInterLoadEtc] int 版实测反劣 +2/分支（4.4 传 uchar 形参前多
                // 一次 movzbl 截断到临时槽）；保持 uchar 与 ORIG 传参值域一致。
                unsigned char logState = user->GetLogInOutState();
                // [BigInterLoadEtc] ORIG 0x084c07f9..0x084c0862 实参右→左：
                // serverGroup/logState 走具名槽，channelNo/port/peerIp 均为内联
                // 实参（GetPeerIP2 返回值即第二实参；channelNo/port 无具名槽）。
                user->m_historyLog.Login(user->getWebAddress(),
                    net->GetPeerIP2(reinterpret_cast<char*>(peerIp + 4), 0x10),
                                         (int)(net->get_port() & 0xffff),
                                         G_CEnvironment()->m_serverEnvir.m_channelNo,
                                         logState,
                                         serverGroup);
            }
            else
            {
                // [BigInterLoadEtc 修复] ORIG 0x084c086c..0x084c088a：else 分支先
                // 清零前 16 字节（4×`movl $0x0,-0xb0/-0xac/-0xa8/-0xa4(%ebp)`）。
                peerIp[0] = 0;
                peerIp[1] = 0;
                peerIp[2] = 0;
                peerIp[3] = 0;
                int serverGroup = (int)user->GetServerGroup();
                unsigned char logState = user->GetLogInOutState();
                user->m_historyLog.Login(user->getWebAddress(),
                    net->GetPeerIP2(reinterpret_cast<char*>(peerIp), 0x10),
                                         (int)(net->get_port() & 0xffff),
                                         G_CEnvironment()->m_serverEnvir.m_channelNo,
                                         logState,
                                         serverGroup);
            }
        }

        // [BigInterLoadEtc] ORIG 0x084c093d..0x084c096e：getCurSec 结果 eax 直传
        // 实参（无具名槽），两次调用各自重取 ⇒ 内联。
        user->SetLoginTick(GlobalData::s_systemTime_.getCurSec());
        user->SetCharacStartPlayTick(GlobalData::s_systemTime_.getCurSec());

        CSkillTreeParam skillTreeParam(user, sig, packet);

        // ======== 主链（对应 ORIG try/状态码分支）========
        {

            if (G_GameWorld()->IsPvPSkilTreeChannel())
            {
                // [BigUserLogic 修复] 原写 m_mixedRegion[0x11](+0xc9d1)；ORIG
                // 0x084c09b1 为 movzbl 0xcdd1(%eax) = m_scriptVersion。
                unsigned char flagByte = sig->m_scriptVersion;
                unsigned char refByte = (unsigned char)
                    G_CDataManager()->m_pvpSkillTree.m_field50;  // +0xa850 基线标志
                if (flagByte != refByte)
                {
                    int myGrade = user->get_pvp_grade();
                    // ORIG:基线读取与 getInitSkillTreeGrade 调用各自重取 G_CDataManager()。
                    unsigned char initBase =
                        (unsigned char)G_CDataManager()->m_pvpSkillTree.m_field50;
                    // [BigInterLoadEtc 修复] ORIG 0x084c09e6..0x084c09ff 实证：
                    // `movzbl %al,%esi`（initBase）→ `mov %esi,0x4(%esp)` 为
                    // getInitSkillTreeGrade 的实参；`cmp %eax,%ebx` 用 myGrade(%ebx)。
                    // 旧写 `(void)initBase` + 传 myGrade 是参数用错（真语义 bug）。
                    if (myGrade < G_CDataManager()->m_pvpSkillTree
                                           .getInitSkillTreeGrade(initBase))
                    {
                        memset(sig->m_mixedRegion, 0, 0x414);
                        pvpTreeSkip = true;
                    }
                }
            }

            if (!user->SetSkill(reinterpret_cast<SIG_LOAD_SKILL*>(sig->m_mixedRegion),
                                reinterpret_cast<SIG_COMBO_SKILL*>(sig->m_comboSkill)))
            {
                user->disableSaveSkill();
                return 0xb91;   // ORIG 0x08661e?? else 尾：dtor 后返回
            }
            user->disableSaveSkill();

            if (G_GameWorld()->IsPvPSkilTreeChannel())
            {
                // [BigUserLogic 修复] ORIG 0x084c0a8f/0x084c0a9d 布局：give 体为
                // fall-through、Set 体由 jne 跳出 ⇒ 源码是 OR 形判 give。
                if (sig->m_pvpSkillInsertFlag != 0 || sig->m_fairPvpLastPlaySec == 0)
                {
                    user->givePvPSkillTree(0, false, 3);
                    user->givePvPSkillTree(0, true, 3);
                    int grade = user->get_pvp_grade();
                    char secondGrow = user->getCurCharSecondGrowType();
                    char firstGrow = user->getCurCharFirstGrowType();
                    int job = user->get_charac_job();
                    int spPoint = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
                        job, (int)firstGrow, (int)secondGrow, grade, true);
                    // [BigInterLoadEtc] ORIG 0x084c0e5d..0x084c0e9?：getCurCharacSkillW
                    // 结果 eax 直传 this（无具名槽，两处各自重调）⇒ 链式内联。
                    reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
                        ->set_remain_sp_at_index(spPoint, ENUM_SKILL_TREE_KIND(0));
                    reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
                        ->set_remain_sp_at_index(spPoint, ENUM_SKILL_TREE_KIND(1));
                }
                else
                {
                    SetPvPSkillTree(user,
                        reinterpret_cast<const _Mastered_skill*>(
                            sig->m_mixedRegion + 0xc),
                        ENUM_SKILL_TREE_KIND(0));
                    SetPvPSkillTree(user,
                        reinterpret_cast<const _Mastered_skill*>(
                            sig->m_mixedRegion + 0x1a8),
                        ENUM_SKILL_TREE_KIND(1));
                }
            }

            if (skillTreeParam.SetSkillTree(this))
            {
                return 0xbcc;
            }

            {
                // [BigInterLoadEtc] ORIG 0x084c0c??：GetItemLockRefVec 结果在 %ebx、
                // titleBook/两个 itemLock 均 eax 直传（无具名槽）⇒ 全内联。
                reinterpret_cast<CTitleBook*>(user->GetCharacExpandData(
                    ENUM_CHARAC_EXPAND_TYPE(0xe)))->getItemLockList(
                    reinterpret_cast<item_lock::CItemLock*>(user->GetCharacExpandData(
                        ENUM_CHARAC_EXPAND_TYPE(2)))->GetItemLockRefVec());
                reinterpret_cast<item_lock::CItemLock*>(user->GetCharacExpandData(
                    ENUM_CHARAC_EXPAND_TYPE(2)))->VerifyItemLock(user);
            }

            // [BigInterLoadEtc] ORIG 0x084c0c51..0x084c0c56：`mov %al,-0x56;
            // movzbl -0x56,%eax; xor $1; test; je` ⇒ 具名字节局部（头签名 int 需
            // 调用点 P3 收窄，否则 4.4 插 test/setne 归一化多 1 条）。
            unsigned char inserted =
                (unsigned char)G_CGameManager()->insert_game_world(user);
            if (inserted == 0)
            {
                return 0xc14;
            }

            user->sendCharacQp();
            user->sendCharacQuestPiece();
            {
                CBoosterGage* booster = reinterpret_cast<CBoosterGage*>(
                    user->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE(0x11)));
                booster->send_data(user, 0);
            }
            user->SendItemMakingSkillInfo();

            // [BigUserLogic] 原 `ch && (a || b || c)` 合写在 4.4 产 bool 物化 $1/$0
            // 汇合（ORIG 0x084c0cf5+ 为逐段 test/je/cmpb/je 直落）⇒ 拆嵌套。
            if (G_GameWorld()->IsPvPSkilTreeChannel())
            {
                if (sig->m_pvpSkillInsertFlag != 0 ||
                    sig->m_fairPvpLastPlaySec == 0 ||
                    pvpTreeSkip)
                {
                packet.clear();
                packet.put_header(0, 0x154);
                packet.finalize(true);
                user->Send(packet);
                }
            }
            user->CheckMaxLuckyLevel();

            user->SetCurConditionEventStep((short)sig->m_curConditionEventStep);
            user->SetCurConditionEventRewardStep((short)sig->m_curConditionEventRewardStep);
            user->SetCurCharacRevengeData(
                sig->m_curVillageAttackCount,
                sig->m_curRevengeDungeonCount);
            if ((short)sig->m_properDungeonClearCount < 0)
                sig->m_properDungeonClearCount = 0;
            user->SetProperDungeonClearCount((short)sig->m_properDungeonClearCount);
            user->SetCurCharacUsedGiftFatigueQuantity(
                sig->m_curCharacUsedGiftFatigueQuantity);

            if (user->getCurCharacR() != 0)
            {
                user->SetFirstTimeLogin(false);
                int fatigueBase = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
                CCoinRefillEvent* coinEvent = static_cast<CCoinRefillEvent*>(
                    GlobalData::s_event_manager->GetRepeatEvent(0xd));
                // [BigUserLogic] ORIG 无判空：直接 vtable+0x34 调 IsEventing。
                if (coinEvent->IsEventing(0))
                    fatigueBase = coinEvent->getLastEventTime();
                user->setBlueMarbleEnterCount(sig->m_blueMarbleEnterCount);

                // [BigInterLoadEtc] ORIG 0x084c0f50 段：nowTick 在 %ebx、lastPlay 由
                // getCurCharacR()->+0x7b 直读（均无具名槽）⇒ 内联实参（右→左求值）。
                if (CheckDailyScheduleTime(fatigueBase,
                        user->getCurCharacR()->m_lastPlayTick,
                        GlobalData::s_systemTime_.getCurSec()))
                {
                    user->SetFirstTimeLogin(true);
                    user->RecoverCoin(1);
                    user->AddDailyItem();
                    user->ClearCurConditionEventStep();
                    user->ClearCurConditionEventRewardStep();
                    user->ClearProperDungeonClearCount();
                    user->SetFirstLogin();

                    // [BigInterLoadEtc] 具名 bool 局部会落槽（+movb/cmpb 各一），
                    // ORIG 0x084c0f6e 区该汇聚值仅在 %al 物化无槽 ⇒ 改匿名条件。
                    if (user->IsFirstTimeLogin() &&
                        user->getCurCharacGrowType() == 0)
                    {
                        // [BigUserLogic] ORIG 0x084c12??：getHackAnalyzer 结果留
                        // %eax 直传（无槽 spill），单行嵌套形复现。
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0x1a1, 1u, 0, 0);
                    }
                    user->ResetCurCharacUsedGiftFatigueQuantity();

                    int level = user->get_charac_level();
                    if (user->IsFirstTimeLogin() &&
                        user->getCurCharSecondGrowType() == 0)
                    {
                        // [BigUserLogic] ORIG 0x084c13??(secondGrow) 为降序 jle 链 +
                        // unsigned 比较(setb)：>0x3c→(3,0xc)；>0x32→(1,0xb)。
                        unsigned int loops = 0;
                        unsigned int subCode = 0;
                        if (level > 0x3c) { loops = 3; subCode = 0xc; }
                        else if (level > 0x32) { loops = 1; subCode = 0xb; }
                        for (unsigned int i = 0; i < loops; ++i)
                        {
                            user->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0x1a3, 1u, subCode, 0);
                        }
                    }
                    if (user->IsFirstTimeLogin() &&
                        user->getCurCharFirstGrowType() == 0)
                    {
                        // [BigUserLogic] ORIG 降序 jle 链：>0x32→(5,4)；>0x28→(3,3)；
                        // >0x1e→(2,2)；>0x14→(1,1)（与旧嵌套形逐区间等价）。
                        unsigned int loops = 0;
                        unsigned int subCode = 0;
                        if (level > 0x32) { loops = 5; subCode = 4; }
                        else if (level > 0x28) { loops = 3; subCode = 3; }
                        else if (level > 0x1e) { loops = 2; subCode = 2; }
                        else if (level > 0x14) { loops = 1; subCode = 1; }
                        for (unsigned int i = 0; i < loops; ++i)
                        {
                            user->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0x1a3, 1u, subCode, 0);
                        }
                    }
                    user->resetBlueMarbleEnterCount();
                }
                user->sendBlueMarbleEnterCount();
                user->SendOpenflag();
                // ORIG:RewardItem 实参与后续判定各自调用 getCurCharacR()。
                RewardItem(user, user->getCurCharacR()->m_lastPlayTick,
                           0x4b4515f0, 0x4b578af0, 0x27addd,
                           "game_server_msg_53", "game_server_msg_131", 0);
                if (user->getCurCharacR()->m_lastPlayTick != 0)
                    user->initAllSkill();
            }

            user->SendRevengeDungeon();
            user->SendConditionEventInfo();
            user->SendProperDungeonClearCount();
            processSeriaRoomAniDecoEvent(
                *reinterpret_cast<seriaRoom_AniDeco*>(sig->m_seriaRoomAniDeco), *user);
            processCheckInvalidExpPerLevel(*user);
            WongWork::CHandlePremium::handlePremiumEffect(user, ENUM_PREMIUM_TYPE(9999));

            for (int evIdx = 1; evIdx < 2; ++evIdx)
            {
                if (CServerEvent::IsEventing(evIdx))
                {
                    int interval = CServerEvent::GetEventInterval(evIdx);
                    Inter_MonitorServerEventStart::SendEventNotiPacket(
                        true, (short)evIdx, interval, user);
                }
            }
            {
                char winnerSide = GlobalData::s_power_manager->GetWinnerSide();
                packet.clear();
                packet.put_header(0, 0xbc);
                if (G_GameWorld()->IsIntegratedPvPBaseChannel())
                    packet.put_byte(99);
                else
                    packet.put_byte((int)winnerSide);
                packet.finalize(true);
                user->Send(packet);
                // [BigUserLogic] ORIG 无判空。
                if (GlobalData::s_power_manager->CheckCompleteDBLoadStatueInfo())
                    GlobalData::s_power_manager->SendPowerWarUserStatueInfo(user);
            }
            {
                packet.clear();
                GlobalData::s_event_manager->MakeNotiEventInfo(packet, user);
                user->Send(packet);
            }
            skillTreeParam.SendSkillPointReturnInfo();
            user->SendDungeonClearList();
            user->EnableCharacInfo(CACHE_SECTION(0xb));
            _checkUserDisadvantage(user);
            user->onSelectCharacter();

            if (user->NeedSecuReward() && user->getRewardType() != 0)
            {
                unsigned char rewardType = user->getRewardType();
                user->AddSecuRewardItem(rewardType);
                user->setRewardType(0);
            }

            // ORIG:检查与发信各调一次 getPad();判定用 getChangedPwd() 访问器。
            if (user->getPad()->getChangedPwd() != 0)
            {
                user->getPad()->SendChagePwdRewardMail(user);
            }

            user->discard_mouse_register();

            // ORIG:两个判定各自调用 get_charac_level()。
            if (user->get_charac_level() > 0x27)
                DB_LoadBloodBestRecord::makeRequest((int)user->GetUID(),
                                                    user->getCurCharacNo());
            if (user->get_charac_level() > 0x3b)
                DB_LoadDimension::makeRequest((int)user->GetUID(),
                                              user->getCurCharacNo());

            if (user->isBreakAwayRewardData())
                giveBreakAwayReward(user);

            if (user->getBreakAwayDungeonClearCnt() > 0)
            {
                int questTime = user->getBreakAwayQuestTime();
                int remain = questTime -
                             (int)GlobalData::s_systemTime_.getCurSec();
                if (remain < 1)
                {
                    user->setBreakAwayDungeonClearCnt(-1);
                }
                else
                {
                    packet.clear();
                    packet.put_header(0, 0x136);
                    packet.put_int(remain);
                    packet.put_byte(3 - user->getBreakAwayDungeonClearCnt());
                    packet.finalize(true);
                    user->Send(packet);
                }
            }

            {
                // ORIG:判定与双 send 分支各自调用 G_GameWorld()。
                int lvlNow = user->get_charac_level();
                // [BigInterLoadEtc 修复] ORIG 0x084c1279 压栈立即数 0x2aff（非 -1）：
                // `movl $0x2aff,0x4(%esp)` 后 call getDungeonMinimumRequiredLevel。
                int minLvl = G_GameWorld()->getDungeonMinimumRequiredLevel(0x2aff);
                if (lvlNow >= minLvl)
                {
                    if (!G_GameWorld()->IsPVPChannel())
                    {
                        // [BigInterLoadEtc] ORIG 0x084c1297/0x084c1301：正条件
                        // `test %al; je <false 体>` ⇒ open 真值体在前（fall-through）。
                        if (G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff))
                        {
                            G_GameWorld()->send_user_dungeon_inout_message(
                                user, 0x2aff, true);
                        }
                        else
                        {
                            G_GameWorld()->send_user_dungeon_inout_message(
                                user, 0x2aff, false);
                        }
                    }
                }
            }

            {
                // [BigUserLogic] ORIG 无判空；ORIG 0x084c1317-0x084c1322 无具名槽
                // （eax 直传 this）⇒ 链式单语句。
                G_CDataManager()->GetAutoMarketContitionsControl()
                    ->SendNotiSetDurability(user);
            }

            // [BigUserLogic] ORIG 无判空：getEffectNum 直调。
            CGlobalEffectManager* fx = GlobalData::s_globalEffectManager_;
            if (fx->getEffectNum() != 0)
            {
                packet.clear();
                fx->makeGlobalEffectInfo(packet);
                packet.finalize(true);
                user->Send(packet);
            }

            {
                CSharedServerMessageManager* sharedMsg =
                    G_CGameManager()->GetSharedServerMessageManager();
                if (sharedMsg != 0)
                {
                    // [BigUserLogic] ORIG 0x084c17a? 布局：IsSetServerMessage 结果
                    // `test %al; je <清零体在后>` ⇒ 源码为正向 if + then=取 msg。
                    if (sharedMsg->IsSetServerMessage())
                    {
                        EVENT_SERVER_MESSAGE* msg =
                            reinterpret_cast<EVENT_SERVER_MESSAGE*>(
                                sharedMsg->GetCurServerMessage());
                        if (msg != 0)
                        {
                            // ORIG:const_iterator 循环,7 个字段逐一经 operator*() 拷贝
                            // (de 站点 ×6/字节域 ×1;m_count 先整体拷贝)。
                            // [BigUserLogic] 原 dstMsg 命名局部产 1 条基址 spill 槽
                            // + 每写点多 1 条 reload；ORIG 4.1 每写点直发 sig 基址
                            // 变址。改逐点表达式。
                            reinterpret_cast<SIG_EVENT_SERVER_MESSAGE*>(
                                &sig->m_eventMessageCount)->m_count = msg->m_count;
                            if (msg->m_count == msg->m_vecMsg.size())
                            {
                            std::vector<stSERVER_MESSAGE>::const_iterator smi =
                                msg->m_vecMsg.begin();
                            std::vector<stSERVER_MESSAGE>::const_iterator smiEnd =
                                msg->m_vecMsg.end();
                            int idx = 0;
                            while (smi != smiEnd)
                            {
                                // ORIG:7 个字段读各自经 operator*()（de 站点 ×6）。
                                SIG_EVENT_SERVER_MESSAGE* dst =
                                    reinterpret_cast<SIG_EVENT_SERVER_MESSAGE*>(
                                        &sig->m_eventMessageCount);
                                dst->m_msg[idx].m_lenMsg = (*smi).m_lenMsg;
                                dst->m_msg[idx].m_kind = (*smi).m_kind;
                                dst->m_msg[idx].m_endTime = (*smi).m_endTime;
                                // [BigUserLogic 修复] 原用 sizeof(=0x14) 多拷 1 字节；
                                // ORIG 0x084c18e0 push 立即数 0x13。
                                strncpy(dst->m_msg[idx].m_msg2, (*smi).m_msg2, 0x13);
                                dst->m_msg[idx].m_index = (*smi).m_index;
                                // [BigUserLogic 修复] 同上：ORIG 0x084c193d 为 0x31。
                                strncpy(dst->m_msg[idx].m_msg, (*smi).m_msg, 0x31);
                                dst->m_msg[idx].m_lenMsg2 = (*smi).m_lenMsg2;
                                idx = idx + 1;
                                smi++;
                            }
                            }
                        }
                    }
                    else
                    {
                        sig->m_eventMessageCount = 0;
                    }
                    sharedMsg->SendServerMessageInfo(
                        user, reinterpret_cast<SIG_EVENT_SERVER_MESSAGE*>(&sig->m_eventMessageCount), 0);
                }
            }

            // [hc 收] +0x330 为 SIG_LIMIT_ITEM_INFO 在该 SIG_LOAD_ETC 承载体缓冲
            // 中的固定布局偏移；经 view 类型 cast 直读（§5.3 序列化缓冲豁免）。
            user->SetLimitInfo(reinterpret_cast<SIG_LIMIT_ITEM_INFO*>(
                (char*)&sig->m_limitItemCount));
            user->SendLoginToPCRoomServer();

            {
                static_cast<CDataAlias*>(user->GetCharacExpandData(
                    ENUM_CHARAC_EXPAND_TYPE(7)))->loadData(user, sig->m_mixedRegion + 0x55f);
            }

            {
                if (user->GetCurCharacExpertJobType() != 0 ||
                    user->GetCurCharacExpertJobExp() == -1)
                {
                    // [BigInterLoadEtc/P259] ORIG 无 stream 具名局部：Acquire 结果
                    // 直接作 CStreamGuard 实参（eax 直传 0x4(%esp)）。
                    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                        "InterDispatcher.cpp", 0xed7), true);
                    guard.operator*()->operator<<(0x180);
                    guard.operator*()->operator<<((int)user->GetUID());
                    guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
                }
            }

            {
                // ORIG:empty 判定与 makeItemLimitEditionInfo 各自重新调用 getter。
                // ORIG:无 null 判定,empty 与 makeInfo 各调一次 getter。
                if (!G_CDataManager()->getItemLimitEditionMgr()->empty())
                {
                    packet.clear();
                    packet.put_header(0, 0xe1);
                    packet.put_byte(0);
                    G_CDataManager()->getItemLimitEditionMgr()->makeItemLimitEditionInfo(packet);
                    packet.finalize(true);
                    user->Send(packet);
                }
            }

            {
                CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                    "InterDispatcher.cpp", 0xf20), true);
                guard.operator*()->operator<<(0x1a7);
                guard.operator*()->operator<<((int)user->GetUID());
                guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            }

            // ORIG:两个独立 if,各自重调 G_GameWorld()->IsIntegratedPvPBaseChannel()。
            if (G_GameWorld()->IsIntegratedPvPBaseChannel())
            {
                CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                    "InterDispatcher.cpp", 0xf2a), true);
                guard.operator*()->operator<<(0x1d5);
                guard.operator*()->operator<<((int)user->GetUID());
                guard.operator*()->operator<<((char)user->GetServerGroup());
                guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            }
            if (G_GameWorld()->IsIntegratedPvPBaseChannel())
            {
                CStreamGuard guard2(GlobalData::s_stream_pool->Acquire(
                    "InterDispatcher.cpp", 0xf36), true);
                guard2.operator*()->operator<<(0x1e0);
                guard2.operator*()->operator<<((int)user->GetUID());
                guard2.operator*()->operator<<((char)user->GetServerGroup());
                guard2.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard2);
            }

            if (G_GameWorld()->GetChannelType() == 0xe)
            {
                unsigned int playCount =
                    online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam();
                if (playCount == 1)
                {
                    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                        "InterDispatcher.cpp", 0xf44), true);
                    guard.operator*()->operator<<(0x1e6);
                    guard.operator*()->operator<<((int)user->GetUID());
                    guard.operator*()->operator<<((char)user->GetServerGroup());
                    guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
                }
                else
                {
                    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                        "InterDispatcher.cpp", 0xf4d), true);
                    guard.operator*()->operator<<(0x1e8);
                    guard.operator*()->operator<<((int)user->GetUID());
                    guard.operator*()->operator<<((char)user->GetServerGroup());
                    guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
                    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
                }
            }

            user->send_RedeemInfo();
            // [BigUserLogic 修复] ORIG 0x084c222f 判的是 user(param_2)!=0，
            // 不是 manager；原写法把判空对象改错 ⇒ 语义差。
            if (user != 0)
                GlobalData::s_DailyScheduleManager->CheckAndSendToOneUserSchedule(*user);
            user->send_MissionList();
            user->processReturnUser();
            APSystem::CUserProc::SendActionListState(user);
            user->setFlagPresentAvengerTitle(sig->m_flagPresentAvengerTitle != 0);
            user->VerifyPresentAvengerTitle();

            {
                unsigned int cur = (unsigned)
                    GlobalData::s_systemTime_.getCurSec();
                // findGameScript 返回 Arad_InterfaceGameScript*，向下转型到 period 数据管理器
                if (reinterpret_cast<ARAD::Arad_EventPeriodDataManager*>(
                        ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
                            "Arad_EventPeriodDataManager"))
                    ->isApplied(ENUM_REPEAT_EVENT_CODE(0x8e), cur))
                {
                    AradServerStateMessage msg;
                    ARAD::notifyOpenMessageDialog(user, 0, 0x8e, msg);
                }
            }
            RequestDBEventArad(user);

            {
                std::bitset<32> flags(sig->m_bitset);
                // ORIG:每次 process_account_first_login 前重调 get_event_script_mng()。
                static_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())
                    ->process_account_first_login(0x191, user, flags);
                static_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())
                    ->process_account_first_login(0x192, user, flags);
                static_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())
                    ->process_account_first_login(0x19c, user, flags);
            }

            {
                CItemShop* shop = G_CDataManager()->m_itemShop;
                // [BigUserLogic] ORIG 无判空。
                user->SendOneADayItemShopIndex(shop->GetOneADayItemList());
            }
            user->reset_is_end_lethe_at_login();

            {
                // ORIG:两次调用各自重取 get_event_script_mng()。
                static_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())
                    ->send_event_init_data(user, false);
                static_cast<EventClassify::CEventScriptMng*>(
                    G_CDataManager()->get_event_script_mng())
                    ->send_event_init_record_data(user);
            }

            {
                // [BigUserLogic] ORIG 0x084c24??：实参右到左 = cnt(0xc9b7,movzbl%esi)
                // → total(0xc9b3,%ebx) → getCurSec() 调用最后；三命名局部形产 6 条
                // 槽往返，单行直传复现。
                user->setPcRoomPlayTimeData(
                    (unsigned int)GlobalData::s_systemTime_.getCurSec(),
                    sig->m_pcRoomPlayTime.m_totalTime,
                    sig->m_pcRoomPlayTime.m_rewardCnt);
            }
            user->set_aura_avatar_option(*reinterpret_cast<stAuraAvatarOption*>(
                sig->m_mixedRegion + 0xd70));
            user->send_aura_avatar_option();
            // [BigUserLogic] ORIG 无判空。
            GlobalData::s_serverStateManager_->sendState(user);
            user->setCharacOption(*reinterpret_cast<CharacterOption*>(
                sig->m_mixedRegion + 0x2108));
            user->sendCharacOption();
            if (sig->m_fieldC != 0)
                user->SendTagCharacInfo();

            const_cast<_Charac_info*>(user->getCurCharacR())->m_tagCharacInfoSent = 0;   // _Charac_info pad 拆分后 +0x14bc 具名字段

            {
                // [BigUserLogic] ORIG 无判空；ORIG 0x084c2169-0x084c2173 无具名槽。
                G_CGameManager()->GetCraneMinigameManager()
                    ->SendCraneItemNeedMaterial(user);
            }
            processAttendAnceEvent(&sig->m_attendanceEvent, *user);
            user->SendMaxEquipLevel();
            processGrowthWeaponEvent(*user);
            processGrowthCreatureEvent(*user);
            user->sendBingoAddData();
            user->requestCubePremiumInfoIfContracted();
            CerashopAddRestrict::Manager::LoadRequestBuyRestrictItem(user);

            {
                // [BigUserLogic] ORIG 无判空；ORIG 0x084c2207-0x084c2213 无 ev 槽
                // （GetRepeatEvent 结果 eax 直传虚调用）。
                if (GlobalData::s_event_manager->GetRepeatEvent(0x9e)->IsEventing(0))
                    DB_LoadEventLevelupSupport::makeRequest((int)user->GetUID(),
                                                            user->getCurCharacNo());
            }
            {
                // [BigUserLogic] ORIG 无判空；ORIG 0x084c2226-0x084c2229 无具名槽。
                ARAD::Singleton<HeroMissionEvent>::Get()->loadEventData(user);
            }

            // ORIG:Garena 段(双 getGarenaAuthData + Taiwan::GarenaAuthData 访问器)。
            // CUser::getGarenaAuthData 返回 char*(GarenaAuthData 视图首址)。
            Taiwan::GarenaAuthData* garenaData =
                reinterpret_cast<Taiwan::GarenaAuthData*>(user->getGarenaAuthData());
            if (Taiwan::GarenaCyberCafe(garenaData->getGcaType()) != 0)
            {
                Taiwan::GarenaAuthPCRoom(user);
            }
            if (reinterpret_cast<Taiwan::GarenaAuthData*>(
                    user->getGarenaAuthData())->getMobileAuth() != 0)
            {
                // ORIG 0x815c586:makeRequest 不消费 this(m_rewarded 经栈参 user 读,
                // 无 C1 构造站点)——静态语义调用。
                DBSelectMobileAuthRewardTw::makeRequest(user);
            }

            return user->unlock4DataLoad() ? 0 : 0x1139;
        }
    }
}