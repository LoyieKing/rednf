// ---------------------------------------------------------------------------
// CDataManager::init —— ORIG 0x08358332..0x0835f1d5（0x6ea3 字节，~5430 指令）。
// 报告 docs/class_func_reports/CDataManager/init.md 全量转写（汇编 + 反编译 C
// 逐段核对，含 +0x1124/+0x18f6/+0x2565/+0x2640 等歧义区回查原始 asm）。
//
// 结构：约 160 个脚本装载步骤组成的「成功续链」——每步失败按 ORIG 各自的
// 行为处理：多数 TRACE 后 return 0；个别仅告警继续（ItemInfoMaster、
// GoldLimit、bindsphere、TowerOfDespair、CheckLightServer/Statistic 等，
// 均在对应分支注释标注报告行号）。
//
// 失败路径返回 0；全链成功返回 1。栈局部对象（vector<PVPMAP_DATA>、
// vector<_DailySchedule>、SDCActiveStatusSyncServer、AvatarRoulette）由 C++
// 作用域析构，与 ORIG 异常清理块（+0x6df2..+0x6e96）等价。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManager DIFF-N] 真语义缺口修复 ×3：① importDailyScheduleEtcScript
// 失败 printf("importDDayEventEtcScript Fail(%d)\n")（ORIG 0x08359eaa）；②
// initWorldMapScript 失败 puts("Load WorldMap.lst fail")（ORIG 0x0835b652）；
// ③ 非空判定改 size()!=0（ORIG 0x08359ec9 调 size，旧 empty() 为 EXTRA）。另消除
// ptEv ×5（EquipmentPartSet 段 data/pieces 具名引用，ORIG 0x0835e4f0/0x0835e4fe）。
// Main 已代落 CItemGeneratorMgr::C1 throw()（CItemGeneratorMgr.h + CtorProducers_2C.cpp）
// ⇒ new-expression landing pad 消除，p167 843/843 CLEAR（0 missing/0 extra）。
// [b28d-BigDataMgrInit 大件收敛] 读数 **orig 5430 / ours 5429**（fn_diff_tu
// norm 5429=5429，gap 0；TU 四数 600/1193/3/10/436 与基线全等）。本轮修：
// ① P248 档① bool 化 ×3 调用点（importChannelScript / CItemUpgrade::LoadUpgradeScript
//    / CItemUpgrade_Separate::LoadUpgradeScript）：ORIG `call; xor $1,%eax;
//    test %al,%al; je` = bool 取反形，旧 `&1` 等价改写每处多 2 条（探针
//    /tmp/probe_xor.cpp 六形实证 int callee 无解）；声明+定义同步 bool，callee
//    机器码零变化（返回语句全常量；W6/Sep/CtorProducers_2C 三件 10/10、11/11）。
// ② P310 循环尾测：`for (int i=0;(unsigned int)i<=6;++i)` 复现两段寄存器形。
// ③ 帧对齐：char _pad[16] 撑帧后 sub $0x192c 与 ORIG 全等、ver 槽
//    -0xf1c..-0xf0c 对齐（P232）。
// 残差（封顶）：a) cMyTrace 临时 arena 槽号差 0x10（90 行，N95/P182：4.1 基址
// -0xf08 vs 4.4 -0xef8，帧已等仍不随帧移）；b) 0x0835cecb/0x0835cfcb 出厂单字节
// 改写 `eb 56`(jmp) 应为 `74 56`(je)，失败块不可达——合法 C++ 不可造（N111 类）；
// c) EquipmentPartSet 段 data/pieces 引用槽序互换 1 对（N93，交换声明序实测
// 反劣 5431/5432 + 658 行非槽差，已回滚）。
// [b28d-solo3 残差复核] 槽归一化后本件仅 **3** 处残差：
//   ① 我方多 8 条 new-expression landing pad（`m_itemGeneratorMgr = new
//      WongWork::CItemGeneratorMgr;` 处 `jmp`→`call _ZdlPv`→`_Unwind_Resume` 段）。
//      根因 = `CItemGeneratorMgr() throw() {}` 体内构造成员 m_common，而
//      `CGeneratorCommonData()`（CItemGenerator.h:170）声明**无** throw() 且体在
//      他处 ⇒ 4.4 判 C1 可能抛（探针 /tmp/probe_pad2.cpp 实证：成员 ctor 无
//      throw() 时外层 throw() 不阻止 landing；成员 ctor 有 throw() 时零 landing）。
//      ⇒ **.h 侧修法（属主代落，P216）**：给 CGeneratorCommonData() 加 throw()。
//   ② ③ 0x0835cecb / 0x0835cfcb ORIG 出厂单字节改写 `eb 56`(jmp) 应为
//      `74 56`(je)（失败块不可达）⇒ 合法 C++ 不可造（N111 类）。
bool CDataManager::init()
{
    // ORIG 版本五元组落在具名区最浅端（槽 -0xf1c/-0xf18/-0xf14/-0xf10/
    // -0xf0c，紧邻临时 arena 基址 -0xf08）；标量局部在本 GCC 下会进顶部
    // 表达式临时槽（实测 -0x5c..-0x6c），只有聚合体（数组）才拿到具名帧槽
    // ⇒ 原函数以 int[5] 形式声明，常数下标折叠后访存形态与标量完全一致。
    // [frame-align] 本侧帧比 ORIG 浅 0x10（0x191c vs 0x192c）；未引用的
    // int[4]（16B）撑帧后帧 0x192c 与 ORIG 全等、ver 五元组槽 -0xf1c..-0xf0c
    // 对齐（P232：-O0 未读写局部照样占 sub；char[16] 实测同形）。
    int _pad[4];
    int ver[5];
    // ORIG 的 5 个函数体作用域 int（按声明序取深→浅槽 -0x3c/-0x38/-0x34/-0x30/
    // -0x2c；两个循环计数器在子块里，故槽号更浅 -0x28/-0x24）。err 为多站点复用
    // 的工作变量（ORIG 在 17 处 `movl $0x0,-0x38` 后接独立赋值+判定两段形）。
    int errDT, err, errPrem, errRO, errMerc;

    // ORIG:每站点独立调用 G_CEnvironment()(无 env 缓存)。

    // [ORIG +0x000c..+0x005d] 遍历 m_serverEnvir.m_priorityScriptDir(+0x2fc)
    // 逐个 SetPriorityBaseDirectory。
    for (unsigned int i = 0; i < G_CEnvironment()->m_serverEnvir.m_priorityScriptDir.size(); ++i)
    {
        SetPriorityBaseDirectory(
            G_CEnvironment()->m_serverEnvir.m_priorityScriptDir[i].c_str());
    }

    // [ORIG +0x005f] m_characNameFilterPass(+0xa8d8) = 0。
    m_characNameFilterPass = false;

    // [+0x0069..+0x00b1] InitRDARScript(基目录 c_str(), m_scriptpacks 字节,
    // GetScriptPacks(), NULL)。
    InitRDARScript(G_CEnvironment()->m_serverEnvir.m_baseScriptDirectory.c_str(),
                   G_CEnvironment()->m_serverEnvir.m_scriptpacks,
                   G_CDataManager()->GetScriptPacks(), (int*)0);

    // [+0x00b3..+0x0176] InitRDARScriptStringManager("n_String.lst", NULL, NULL)
    // —— 失败是唯一的前导致命错误（TRACE 0x1972 / 0x1975）。
    // 两处 TRACE 均为匿名临时对象（ORIG 槽 -0xf08/-0xef8 随站点递增 ⇒ 临时
    // arena 形态，具名对象则反向递增，探针 probe3 实证），故用链式临时写法。
    if (!InitRDARScriptStringManager("n_String.lst", (const char*)0,
                                     (std::vector<ErrorLog>*)0))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1972, 9, true, true)(
            "\t- InitRDARScriptStringManager Fail");
        return 0;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x1975, 9, true, true)(
        "\t- InitRDARScriptStringManager Success");

    // [+0x0176] InitMapEquipmentAniScript()。
    InitMapEquipmentAniScript();

    // [+0x017b..+0x02a8] ItemInfoMaster：失败/DB 步骤失败均只 TRACE 继续
    // （0x1982/0x1986/0x1989）。
    // ORIG:每站点独立 GetInstanceSyncScript()；分支布局为「失败 TRACE 在前
    // （fall-through）、成功链在后（+0x01e8 起）」——故源码用 if(!ok){TRACE}
    // else{…} 形式，站点顺序才是 0x1982 → 0x1986 → 0x1989（报告 +0x0195 je）。
    if (!sync_script::GetInstanceSyncScript()->LoadItemInfoMasterScript("Etc/ItemTypeMaster.etc"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1982, 9, true, true)(
            "\t- Failed to load < ITEM INFO MASTER SCRIPT >");
    }
    else
    {
        if (!sync_script::GetInstanceSyncScript()->truncate_item_info_master_table())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1986, 9, true, true)(
                "\t- Failed to < TRUNCATE ITEM INFO MASTER TABLE >");
        }
        if (!sync_script::GetInstanceSyncScript()->insert_item_info_master_to_db())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1989, 9, true, true)(
                "\t- Failed to < INSERT ITEM INFO MASTER TO DB >");
        }
    }

    // [+0x02a8..+0x03xx] OldEquipmentStatInfo：同上 log-only（0x198f/93/96）。
    // 同上：失败 TRACE fall-through 在前（报告 +0x02b9 je）。
    if (!importOldEquipmentStatInfo(""))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x198f, 9, true, true)(
            "\t- Failed to import < OldEquipmentStatInfo.dat >");
    }
    else
    {
        if (!sync_script::GetInstanceSyncScript()->truncate_old_equip_info_table())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1993, 9, true, true)(
                "\t- Failed to < TRUNCATE OLD EQUIP INFO TABLE >");
        }
        if (!sync_script::GetInstanceSyncScript()->insert_old_equip_info_to_db())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1996, 9, true, true)(
                "\t- Failed to < INSERT OLD EQUIP INFO TO DB >");
        }
    }

    // ---- 主成功续链开始：任一 FAIL 分支 TRACE 后 return 0 ----

    // ServerParameter.etc（fail TRACE 0x1972 区尾段 0x1a35）。
    if (!importServerParameterScript(&m_serverParameter,
                                     "Etc/ServerParameter.etc"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1a35, 9, true, true)(
            "\t- importServerParameterScript Fail");
        return 0;
    }

    // serverType==2 时把 expGroup378 拷入 expGroup368（+0x368/+0x378 各 0x10）。
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 2)
    {
        memcpy(m_serverParameter.m_expGroup368,
               m_serverParameter.m_expGroup378, 0x10);
    }

    // IngameAdvertisementParameter.etc（fail 0x1a4a）。
    if (!importInGameAdvertisementEtcScript(&m_inGameAd,
                                            "Etc/IngameAdvertisementParameter.etc"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1a4a, 9, true, true)(
            "\t- importInGameAdvertisementEtcScript Fail");
        return 0;
    }

    // SeriaRoomDecoEvent.evt（fail 0x1a53）。
    if (!importSeriaRoomDecoEvent(&m_seriaRoomDeco,
                                  "Event/SeriaRoomDecoEvent.evt"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1a53, 9, true, true)(
            "\t- SeriaRoomDecoEventScript Fail!");
        return 0;
    }

    // GrowthWeapon.evt（fail "GrownEquipmentScript Fail!"）。
    if (!importGrowthEquipmentScript(&m_growthWeapon, "Event/GrowthWeapon.evt"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1a5b, 9, true, true)(
            "\t- GrownEquipmentScript Fail!");
        return 0;
    }

    // CreateCharacterGift.evt。
    if (!importCharacterCreateGiftScript(&m_createChrac,
                                         "Event/CreateCharacterGift.evt"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1a63, 9, true, true)(
            "\t- CharacterCreateGiftScript Fail!");
        return 0;
    }

    // JobFatigue.evt。
    if (!importUseJobFatigueGiftScript(&m_useJobFatigue, "Event/JobFatigue.evt"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1a6b, 9, true, true)(
            "\t- UseJobFatigueGiftScript Fail!");
        return 0;
    }

    // CreateCharacterGiftCreature.evt。
    if (!importCharacterCreateCreatureGiftScript(
            &m_createCreatureGift, "Event/CreateCharacterGiftCreature.evt"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1a73, 9, true, true)(
            "\t- CharacterCreateCreatureGiftScript Fail!");
        return 0;
    }

    // SeriaRoomDecoAnimation.evt。
    if (!importSeriaRoomAnimationDecoEvent(&m_seriaAniDeco,
                                           "Event/SeriaRoomDecoAnimation.evt"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1a7b, 9, true, true)(
            "\t- SeriaRoomAnimationDecoEventScript Fail!");
        return 0;
    }

    // EventReward/reward.evt（返回 0=成功）。
    if (m_eventReward.importRewardTable("Event/EventReward/", "reward.evt") != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1a83, 9, true, true)(
            "\t- JumpingCharacterEventScript Fail!");
        return 0;
    }

    // antibot 版本号：读 m_versionBytes 四字节计算后写 m_antibotVersion。
    // ORIG +0x082b..+0x08f2（报告 init.md）逐条转写：5 个 int 局部
    //（ORIG 槽 -0xf1c/-0xf18/-0xf14/-0xf10/-0xf0c，按声明序自深向浅），
    // 每字节读取各自独立调 G_CDataManager()（6 次 call 实证）；
    // hi = byte/100（QI 模式 magic 41：mov $0x29/mul/shr 8/shr 4/movzbl）、
    // lo = byte - hi*100（ORIG `imul $0xffffff9c` + `lea` 实证，非 %100）。
    // 求值序：表达式第 2 组按 (hi*1e4, lo*100) 书写 ⇒ 汇编先 lo→ecx 后
    // hi→edx（右操作数先求值，P23/P24 口径）。
    ver[0] = G_CDataManager()->m_serverParameter.m_versionBytes[0];
    ver[1] = G_CDataManager()->m_serverParameter.m_versionBytes[1];
    ver[2] = G_CDataManager()->m_serverParameter.m_versionBytes[2] / 100;
    ver[3] = G_CDataManager()->m_serverParameter.m_versionBytes[2] - ver[2] * 100;
    ver[4] = G_CDataManager()->m_serverParameter.m_versionBytes[3];
    G_CDataManager()->m_serverParameter.m_antibotVersion =
        ver[0] * 10000000 + ver[1] * 100000 + ver[2] * 10000 + ver[3] * 100 +
        ver[4];
    cMyTrace(__PRETTY_FUNCTION__, 0x1aa7, 9, true, true)(
        "\t- Server version for antibot : %d",
        G_CDataManager()->m_serverParameter.m_antibotVersion);

    // ImageCommunication.etc。
    if (!importImageCommunicationInfo("Etc/ImageCommunication.etc",
                                      &m_imageCommData))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1aad, 9, true, true)(
            "\t- Import ImageCommunicationInfo Failed\n");
        return 0;
    }

    // CheckTraningFirstGive 为假 → TRACE 0x1ab7 后 return 0。
    if (!G_CDataManager()->m_serverParameter.CheckTraningFirstGive())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ab7, 9, true, true)(
            "\t- [training first give] error");
        return 0;
    }

    // expertjob.lst（失败 TRACE 0x1abf）。
    if (!initExpertJobScript((char*)"Character/", (char*)"expertjob.lst"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1abf, 9, true, true)(
            "\t- Load expertjob.lst fail");
        return 0;
    }

    // new CExpertJobList(0x48) → m_regeneration.m_expertJobList；init() 失败 TRACE 0x1ac5。
    m_regeneration.m_expertJobList = new CExpertJobList;
    if (!m_regeneration.m_expertJobList->init())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ac5, 9, true, true)(
            "\t- Load expertjob fail");
        return 0;
    }

    // TowerList.lst（返回 0=成功）。
    if (importTowerInfo("Etc/", "TowerList.lst", m_deathTowerMap) != 0)
    {
        // ORIG +0x0b7a：puts("importTowerInfo Faile")（PLT 0x0807e570 → puts，
        // GOT 0x0936cf34 实证；原文案拼写即少一个 d），非 cMyTrace。
        puts("importTowerInfo Faile");
        return 0;
    }

    // DeathTower.etc（返回 0=成功）。
    // ORIG +0x0b90：errDT 置 0 → `if ((errDT = importDeathTowerScript(..)) != 0)`
    // （store+cmpl+setne），TRACE 带 errDT。
    errDT = 0;
    if ((errDT = importDeathTowerScript("Etc/DeathTower.etc", m_deathTower)) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ad4, 9, true, true)(
            "\t- importDeathTowerScript Fail(%d)", errDT);
        return 0;
    }

    // RankSystemInfo.etc。
    if (!importRankSystemScript("Etc/RankSystemInfo.etc", &m_rankSystem))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ada, 9, true, true)(
            "\t- importRankSystemScript Fail");
        return 0;
    }

    // BloodClearReward.etc（返回 0=成功）。
    err = 0;
    if ((err = importBloodDungeonScript("Etc/BloodClearReward.etc", m_bloodDungeon)) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ae1, 9, true, true)(
            "\t- importBloodDungeonScript Fail(%d)", err);
        return 0;
    }

    // HellParty.etc（全局单例，返回 0=成功）。
    err = 0;
    if ((err = importHellPartyScript( "Etc/HellParty.etc", *reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t()))) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ae8, 9, true, true)(
            "\t- importHellPartyScript Fail(%d)", err);
        return 0;
    }

    // CharacLinkSystem.etc（返回 0=成功）。
    err = 0;
    if ((err = importCharacLinkSystemScript("Etc/LinkSystem/CharacLinkSystem.etc", m_characLink)) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1af3, 9, true, true)(
            "\t- importCharacLinkSystemScript Fail(%d)", err);
        return 0;
    }

    // striker.etc（getInsance 单例存入 m_altarConfig.m_strikerScript）。
    m_altarConfig.m_strikerScript = StrikerScript::getInsance();
    if (!m_altarConfig.m_strikerScript->importScript("Etc/LinkSystem/striker.etc"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1afb, 9, true, true)(
            "\t- importStrikerScript Fail");
        return 0;
    }

    // CharacterManage.etc。
    m_altarConfig.m_characManageScript = CharacManageScript::getInsance();
    if (!m_altarConfig.m_characManageScript->importScript(
            "Etc/LinkSystem/CharacterManage.etc"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b02, 9, true, true)(
            "\t- importCharacterManageScript Fail");
        return 0;
    }

    // BlueMarble 三连（Tile→Map→Info），单例存入 m_choiceItem.m_blueMarbleScriptMgr。
    m_choiceItem.m_blueMarbleScriptMgr = BlueMarbleScriptManager::getInsance();
    if (!m_choiceItem.m_blueMarbleScriptMgr->initTileList(
            "Event/BlueMarble/", "BlueMarbleTile.lst"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b14, 9, true, true)(
            "\t- BlueMarble initTileList Fail\n");
        return 0;
    }
    if (!m_choiceItem.m_blueMarbleScriptMgr->initMapList(
            "Event/BlueMarble/", "BlueMarbleMap.lst"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b1a, 9, true, true)(
            "\t- BlueMarble initMapList Fail\n");
        return 0;
    }
    if (!m_choiceItem.m_blueMarbleScriptMgr->initBlueMarbleInfo(
            "Event/BlueMarble/", "BlueMarble.blu"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b20, 9, true, true)(
            "\t- BlueMarble initBlueMarbleInfo Fail\n");
        return 0;
    }

    // questshop.etc（ORIG +0x10ba：`movl $0x0,-0x38` 后独立赋值+判定两段形，
    // TRACE 带 err 实参）。err 为函数体作用域变量，此处仅复位。
    err = 0;
    if ((err = importQuestShopScript("Etc/questshop.etc", m_questShop)) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b4f, 9, true, true)(
            "\t- importQuestShopScript Fail(%d)", err);
        return 0;
    }

    // IndependentDrop 三连（lst 加载 bool / 列表加载 / 数据导入返回 0=成功；
    // 后两步 fail 均 TRACE "Load IndependentDrop.lst fail" 0x1b56/0x1b5c）。
    if (!initIndependentDropScript((char*)"Etc/", (char*)"IndependentDrop.lst"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b56, 9, true, true)(
            "\t- Load IndependentDrop.lst fail");
        return 0;
    }
    if (!m_independentDrop.load_independent_drop_list())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b5c, 9, true, true)(
            "\t- Load IndependentDrop.lst fail");
        return 0;
    }
    err = 0;
    if ((err = m_independentDrop.ImportIndependentDrop("Etc/Independent_Drop.etc")) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b63, 9, true, true)(
            "\t- importIndependent_DropScript Fail(%d)", err);
        return 0;
    }

    // Event 脚本簇（SeriaBlessing/LevelUpReward/GrowthCapsule/NewAccountLevelUp/
    // ReturnUserReward/Bingo/BroadCastItem，全部返回 0=成功）。
    err = 0;
    if ((err = ImportSeriaBlessingScript("Event/SeriaBlessing.evt", m_seriaBlessing)) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b83, 9, true, true)(
            "\t- ImportSeriaBlessingScript Fail(%d)", err);
        return 0;
    }
    err = 0;
    if ((err = ImportLevelUpRewardItemScript("Event/LevelUpRewardItem.evt", m_levelUpReward)) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1b8a, 9, true, true)(
            "\t- ImportLevelUpRewardItemScript Fail(%d)", err);
        return 0;
    }
    // ORIG +0x13c0..+0x1405：int err = 0；err 由条件内赋值（+0x13df `mov
    // %eax,-0x38` + `setne` 形态，probe7 v7_c 实证），失败走
    // printf("ImportGrowthCapsuleScript Fail(%d)\n", err)（PLT 0x0807db60）。
    err = 0;
    if ((err = ImportGrowthCapsuleScript("Event/GrowthCapsule.evt", m_growthCapsule)) != 0)
    {
        printf("ImportGrowthCapsuleScript Fail(%d)\n", err);
        return 0;
    }
    err = 0;
    if ((err = ImportNewAccountLeventUpToJobScript("Event/NewAccountLevelUpToJob.evt", m_newAccountLevelUp)) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ba7, 9, true, true)(
            "\t- NewAccountLevelUpToJob StackableReward Fail(%d)", err);
        return 0;
    }
    err = 0;
    if ((err = ImportReturnUserRewardScript("Event/ReturnUserReward.evt", m_returnUserReward)) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1bb0, 9, true, true)(
            "\t- ReturnUserReward Fail(%d)", err);
        return 0;
    }
    err = 0;
    if ((err = ImportBingoScript("Event/BingoEvent.evt", m_bingo)) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1bbb, 9, true, true)(
            "\t- ImportBingoScript Fail(%d)", err);
        return 0;
    }
    err = 0;
    if ((err = ImportBroadCastItemScript("Etc/BroadCastItem.etc", m_broadCastItem)) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1bcf, 9, true, true)(
            "\t- ImportBroadCastItemScript Fail(%d)", err);
        return 0;
    }

    // SecurityReward（bool 语义）。
    if (!ImportSecurityReward(&m_securityReward))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1bd7, 9, true, true)(
            "\t- ImportSecurityReward Fail");
        return 0;
    }

    // ItemMakingSkill.etc（返回 0=成功）。
    err = 0;
    if ((err = ImportItemMakingSkill("Etc/ItemMakingSkill.etc", m_itemMakingSkill)) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1be0, 9, true, true)(
            "\t- importItemMakingSkillScript Fail(%d)", err);
        return 0;
    }

    // UltimateDungeonList.etc。
    if (!importUltimateDungeonList("Etc/UltimateDungeonList.etc",
                                   m_ultimateDungeonList))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1be8, 9, true, true)(
            "\t- import UltimateDungeonList Script Fail(%d)");
        return 0;
    }

    // mapping equip info：truncate 成功才继续（失败无 TRACE ret0）；
    // 两张表 false/true 导入（fail TRACE "Equipment Script mapping table
    // load fail" 0x1c0c/0x1c12）；insert 用第二张表（ORIG +0x18f6 取 +0xa940）。
    // 帧差备忘（P31 类）：ORIG 站点 0x1be8→0x1c06 之间槽号跨 0x20（-0xc38 在
    // 全函数无任何引用 ⇒ 原码此处有一个未引用 16 字节局部）。实测：无论以
    // {} 子作用域还是函数体作用域补一个 POD dummy，都只使帧 +0x10 而不产生
    // arena 内 gap ⇒ 该槽来自带非平凡析构的对象（本 TU 无法以合法 C++ 复现），
    // 归入结构性差异，不强凑。
    if (!sync_script::GetInstanceSyncScript()->truncate_mapping_equip_info_table())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1c06, 9, true, true)(
            "\t- Fail truncate_mapping_equip_info_table");
        return 0;
    }
    if (!importExperienceItemMapInfo(m_mapA928, false))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1c0c, 9, true, true)(
            "\t- Equipment Script mapping table load fail");
        return 0;
    }
    if (!importExperienceItemMapInfo(m_mapA940, true))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1c12, 9, true, true)(
            "\t- Equipment Script mapping table load fail");
        return 0;
    }
    if (!sync_script::GetInstanceSyncScript()->insert_mapping_equip_info_to_db(m_mapA940))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1c18, 9, true, true)(
            "\t- Fail insert_mapping_equip_info_table");
        return 0;
    }

    // VillageAttacked.etc（返回 0=成功）。
    err = 0;
    if ((err = importVillageAttackedScript("Etc/VillageAttacked.etc", m_villageAttacked)) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1c20, 9, true, true)(
            "\t- importVillageAttackedScript Fail(%d)", err);
        return 0;
    }

    // ActionPoint 两份（普通/GM；GetActionPointEtcParameter(bool) W 0x08122ace）。
    // ORIG +0x19f4..+0x1a2a：每份各起 err 复位（复用同一临时槽），判定为
    // store+cmpl+setne 两段形态；访问器结果不经局部变量、直接由 %eax 落到
    // 实参槽（不留具名指针）。
    err = 0;
    if ((err = APSystem::CActionPointEtcParameter::importActionPointSystemEtcScript( "Etc/Chn_ActionPointSystem.etc", *GetActionPointEtcParameter(false))) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1c3a, 9, true, true)(
            "\t- importActionPointSystemEtcScript Chn_ActionPointSystem.etc Fail(%d)",
            err);
        return 0;
    }
    err = 0;
    if ((err = APSystem::CActionPointEtcParameter::importActionPointSystemEtcScript( "Etc/Chn_ActionPointSystem_GM.etc", *GetActionPointEtcParameter(true))) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1c41, 9, true, true)(
            "\t- importActionPointSystemEtcScript Chn_ActionPointSystemGM.etc Fail(%d)",
            err);
        return 0;
    }

    // action point desc DB 同步（无条件）。
    sync_script::GetInstanceSyncScript()->truncate_charac_action_point_desc();
    sync_script::GetInstanceSyncScript()->insert_charac_action_point_desc();

    // Chn_ServerCheckSchedule.etc → vector<_DailySchedule> → AddDailSchedule。
    // ORIG 清理梯 +0x6e88：schedules 为函数体作用域对象，析构在函数末尾统一
    // 执行（此后所有 return 0 都经其 dtor），故此处不加嵌套块。
    // err 复位（ORIG +0x1b38 `movl $0x0,-0x38(%ebp)` 先于 vector 构造，
    // AdvanceAltar 四段复用同一变量）。
    err = 0;
    std::vector<_DailySchedule> schedules;
    // [b28d-solo-CDataManager 真语义缺口修复] ORIG 0x08359e95-0x08359ebb：
    // err(-0x38) = importDailyScheduleEtcScript(...)；err!=0 时 **printf**
    // ("importDDayEventEtcScript Fail(%d)\n", err) 再 ret0（旧码无 printf，
    // p167 MISSING printf ×1）；且非空判定为 `size() != 0`（ORIG
    // 0x08359ec9 调 vector<_DailySchedule>::size，旧码 empty() 为 EXTRA）。
    if ((err = importDailyScheduleEtcScript(
             "Etc/Chn_ServerCheckSchedule.etc", schedules)) != 0)
    {
        printf("importDDayEventEtcScript Fail(%d)\n", err);
        return 0;   // ORIG 清理链经 vector dtor 后 ret0（+0x6e88 区）
    }
    if (schedules.size() != 0)
    {
#if !remake_verify
        // Client HUD 领主之塔文案：每天下午 16:00~18:00、每天晚上 21:00~24:00。
        // etc 原文是 00:00-23:59；按客户端文案改 hour（type 4/0 下午，type 1 晚上）。
        for (unsigned int si = 0; si < schedules.size(); ++si)
        {
            _DailySchedule& e = schedules[si];
            if (e.m_type == 4 || e.m_type == 0)
            {
                e.m_startHour = 16; e.m_startMin = 0; e.m_startSec2 = 0;
                e.m_endHour = 18; e.m_endMin = 0; e.m_endSec2 = 0;
            }
            else if (e.m_type == 1)
            {
                e.m_startHour = 21; e.m_startMin = 0; e.m_startSec2 = 0;
                e.m_endHour = 23; e.m_endMin = 59; e.m_endSec2 = 59;
            }
        }
#endif
        GlobalData::s_DailyScheduleManager->AddDailSchedule(schedules);
    }

    // AdvanceAltar 三件套 + TimeLine：ORIG +0x1bc7..+0x1ca7 四处共用一个
    // int err（-0x38 槽，`cmpl $0,-0x38` 直接分支、无 setne ⇒ 形如
    // `err = f(); if (err != 0)`），失败均 printf("<fn> Fail(%d)\n", err) 后
    // ret0（PLT 0x0807db60 = printf）。
    // [b28d-orig-int] ORIG 四站点为 `store; cmpl $0; je` 两段形（无 setne）：
    // `err = f(); if (err != 0)`，与 DailySchedule 站点（单表达式 setne 形）相反。
    err = loadAdvanceAltarTimeLineParameter(m_stageTimeLine);
    if (err != 0)
    {
        printf("loadAdvanceAltarTimeLineParameter Fail(%d)\n", err);
        return 0;
    }
    err = m_altarShop.importAdvanceAltarShopParameter( "Etc/AdvanceAltar/AdvanceAltarShop.etc");
    if (err != 0)
    {
        printf("importAdvanceAltarShopParameter Fail(%d)\n", err);
        return 0;
    }
    err = m_altarReward.importAchievementRewardParameter( "Etc/AdvanceAltar/AdvanceAltarAchievementReward.etc");
    if (err != 0)
    {
        printf("importAchievementRewardParameter Fail(%d)\n", err);
        return 0;
    }
    err = m_altarConfig.importConfigParameter( "Etc/AdvanceAltar/AdvanceAltarConfig.etc");
    if (err != 0)
    {
        printf("importConfigParameter Fail(%d)\n", err);
        return 0;
    }

    sync_script::GetInstanceSyncScript()->truncate_charac_advance_altar_item_desc();
    sync_script::GetInstanceSyncScript()->insert_charac_advance_altar_item_desc();

    // pvpChannelList.etc（live_server_info 宿主）。
    if (!importPVPChannelList((char*)"Etc/pvpChannelList.etc",
                              &m_liveServerInfo, 0))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1cd5, 9, true, true)(
            "\t- importPVPChannelList Fail()");
        return 0;
    }

    // Guild/Power/SecretShop/AccountCargo/TitleBook 参数链。
    if (!importGuildParameterScript(&m_guildParameter, "Etc/Guild.etc"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ce2, 9, true, true)(
            "\t- importGuildParameterScript Fail");
        return 0;
    }
    if (!importPowerParameterScript(&m_powerParameter, "Etc/Power.etc"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ce9, 9, true, true)(
            "\t- importPowerParameterScript Fail");
        return 0;
    }
    if (!importSecretShopScript(&m_secretShop, "Etc/SecretShop.etc"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d0f, 9, true, true)(
            "\t- importSecretShopScript Fail");
        return 0;
    }
    if (!importAccountCargoScript(&m_accountCargo, "Etc/AccountCargo.etc"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d16, 9, true, true)(
            "\t- importAccountCargoScript Fail");
        return 0;
    }
    if (!importTitleBookEtcScript("Etc/TitleBook.etc", &m_titleBook))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d1e, 9, true, true)(
            "\t- importTitleBookEtcScript Fail");
        return 0;
    }

    // EventCharacter 两步。
    if (!initEventCharacterScript((char*)"Event/EventCharacter/",
                                  (char*)"EventCharacter.lst"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d38, 9, true, true)(
            "\t- Load EventCharacter.lst fail");
        return 0;
    }
    if (!m_eventCharacter.load_event_character_list())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d3e, 9, true, true)(
            "\t- Load EventCharacter.lst fail");
        return 0;
    }

    // Quest 参数链。
    if (!importQuestParameterScript(&m_questParameter,
                                    "n_Quest/questParameter.etc"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d57, 9, true, true)(
            "\t- importQuestParameterScript Fail");
        return 0;
    }
    if (!importTrainingQuestScript(&m_trainingQuest,
                                   "n_Quest/TrainingQuest.lst"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d61, 9, true, true)(
            "\t- importTrainingQuestScript Fail");
        return 0;
    }

    // Collect Dungeon Statistic 开关（非 0 才开；log-only TRACE 0x1d84）。
    if (m_serverParameter.CheckCollectDungeonStatisticServer(
            G_CEnvironment()->m_serverEnvir.m_serverGroup, G_CEnvironment()->m_serverEnvir.m_channelNo))
    {
        SetCollectDungeonStatistic(true);
        cMyTrace(__PRETTY_FUNCTION__, 0x1d84, 9, true, true)(
            "\t- Collect Dungeon Data Statistic start!");
    }

    // Assault channel 开启。
    pvp_assault::GetInstanceAssaultMgr()->SetAssaultChannel(true);

    // 名字过滤两件套。
    if (!importSlangName((char*)"Etc/slangname.etc", &m_slangName))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d8b, 9, true, true)(
            "\t- slangname.etc file load failed");
        return 0;
    }
    if (!importInvalidName((char*)"Etc/invalidName.etc", &m_invalidChar))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d90, 9, true, true)(
            "\t- invalidName.etc file load failed");
        return 0;
    }

    // new SlangFilter(0x80) → m_invalidChar.m_slangFilter（destroy 经 vtable D0）。
    m_invalidChar.m_slangFilter = new SlangFilter;

    // FairPVPEquipment.etc → m_stackableMap。
    if (!importFairPVPEquipment((char*)"Etc/FairPVPEquipment.etc",
                                &m_stackableMap))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1d99, 9, true, true)(
            "\t- FairPVPEquipment.etc file load failed");
        return 0;
    }

    // GrowthPower / BoosterGage。
    if (!importGrowthPower((char*)"Etc/GrowthPowerRewardBuff.etc",
                           &m_growthPower))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1da1, 9, true, true)(
            "\t- GrowthPowerRewardBuff.etc file load failed");
        return 0;
    }
    if (!importBoosterGage((char*)"Etc/BoosterGage.etc", &m_boosterGage))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1dac, 9, true, true)(
            "\t- BoosterGage.etc file load failed");
        return 0;
    }

    // AICharacter。
    if (!initAICharacterScript("AICharacter/", "AICharacter.lst"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1db9, 9, true, true)(
            "\t- Load AICharacter.lst fail");
        return 0;
    }
    m_aiCharacterList = new CAICharacterList;
    if (!m_aiCharacterList->init())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1dbf, 9, true, true)(
            "\t- Load AI Character fail");
        return 0;
    }

    // Light Server 提示（log-only TRACE 0x1dc5 区）。
    if (m_serverParameter.CheckLightServer(G_CEnvironment()->m_serverEnvir.m_serverGroup))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1dc5, 9, true, true)(
            "\t- Light Server Mode start!");
    }

    // WorldDrop 两张表（无条件导入）。
    ImportWorldDrop(m_worldDrop1, (char*)"Etc/WorldDrop.etc");
    ImportWorldDrop(m_worldDrop2, (char*)"Etc/WorldDropWarArea.etc");

    // PremiumList_New.etc（ORIG +0x2629 区：errPrem 置 0 → `if ((errPrem = ...) != 0)`，
    // 槽 -0x34；TRACE 带行码）。
    errPrem = 0;
    if ((errPrem = WongWork::CUserPremium::LoadPremiumInfo(
             "Etc/PremiumList_New.etc")) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e0f, 9, true, true)(
            "\t- LoadPremiumInfo Fail(Line: %d)", errPrem);
        return 0;
    }

    // 非 light server 必须成功加载 WarArea.etc（fail TRACE 0x1e18 ret0）。
    // [b28d-orig-form] ORIG 报告 idx 1930-1940：两独立 `xor $1; test %al;
    // je 同一续链目标`（&& 组合在 4.4 会物化 `mov $1/$0` 尾块，EXTRA 5 行）
    // ⇒ 原码为嵌套形。
    if (!IsLightServer())
    {
        if (!ImportWarAreaCounter(&m_warAreaCounter))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1e18, 9, true, true)(
                "\t- WarArea.etc file load failed");
            return 0;
        }
    }

    // ---- 大类脚本链（Character→Monster→PassiveObject→Equipment→Stackable→
    //      Map→Dungeon），任一失败 TRACE ret0 ----
    if (!InitCharacterScript("Character/", "Character.lst", "ExpTable.tbl",
                             (const char*)0))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e23, 9, true, true)(
            "\t- Load Character.lst fail");
        return 0;
    }
    if (!InitMonsterScript("Monster/", "Monster.lst",
                           "MonsterApcDifficultyBonus.tbl"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e28, 9, true, true)(
            "\t- Load Monster.lst fail");
        return 0;
    }
    if (!InitPassiveObjectScript("PassiveObject/", "PassiveObject.lst"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e2d, 9, true, true)(
            "\t- Load PassiveObject.lst fail");
        return 0;
    }
    if (!InitEquipmentScript("equipment/", "equipment.lst", "pricetable.tbl"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e33, 9, true, true)(
            "\t- Load equipment.lst fail");
        return 0;
    }
    if (!InitStackableScript("stackable/", "stackable.lst", "pricetable.tbl"))
    {
        // ORIG +0x2928：puts("Load stackable.lst fail")，非 cMyTrace。
        puts("Load stackable.lst fail");
        return 0;
    }
    if (!InitMapScript("Map/", "Map.lst"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e48, 9, true, true)(
            "\t- Load Map.lst fail\n");
        return 0;
    }
    if (!InitDungeonScript((char*)"Dungeon/", (char*)"Dungeon.lst"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e4d, 9, true, true)(
            "\t- Load Dungeon.lst fail\n");
        return 0;
    }

    // QuickParty 奖励管理器 init。
    // ORIG +0x2a35..+0x2a41：失败无 TRACE，直接 ret0。
    if (!G_CGameManager()->GetQuickPartyRewardManager()->init())
    {
        return 0;
    }

    // Quest/Skill。
    if (!initQuestScript("n_Quest/", "Quest.lst"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e5c, 9, true, true)(
            "\t- Load Quest.lst fail\n");
        return 0;
    }
    if (!InitSkillScript("Skill/", "SkillList.lst"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e61, 9, true, true)(
            "\t- Load SkillList.lst fail\n");
        return 0;
    }

    // ItemShop：new(0x24) → m_itemShop；ItemShop.lst + loadItemShopFiles。
    m_itemShop = new CItemShop;
    if (!initItemShopScript((char*)"ItemShop/", (char*)"ItemShop.lst"))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e68, 9, true, true)(
            "\t- Load ItemShop.lst fail\n");
        return 0;
    }
    if (!m_itemShop->loadItemShopFiles())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e6d, 9, true, true)(
            "\t- Load ItemShop file fail\n");
        return 0;
    }

    // PvP Mission：initPvpMissionSystem + new(0x54) + Load_list + Load_EtcParameter。
    // ORIG +0x2c55..+0x2c61：失败无 TRACE，直接 ret0。
    if (!initPvpMissionSystem((char*)"PVP_Mission/", (char*)"Mission.lst"))
    {
        return 0;
    }
    m_pvpMissionSystem = new PvP_MissionSystem;
    if (!m_pvpMissionSystem->Load_list())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e79, 9, true, true)(
            "\t- Mission Script file fail\n");
        return 0;
    }
    if (!m_pvpMissionSystem->Load_EtcParameter())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e7e, 9, true, true)(
            "\t- Mission Parameter Script file fail\n");
        return 0;
    }

    // CashShop 文件名四连（+0x8cb8 / +0xa8c0..0xa8cc）。
    m_scriptPackPath = "Etc/NewCashShop.etc";
    m_packName1 = "CashShop/NewArad_CashShop.shp";
    m_packName2 = "CashShop/Sudden_Shop_1.shp";
    m_packName3 = "CashShop/Sudden_Shop_2.shp";
    m_packName4 = "CashShop/Sudden_Shop_3.shp";

    // Money Limit per Level（fail 仅 TRACE 0x1eb7 继续）。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1eb4, 9, true, false)(
            "\t- Loading Money Limit per Level ");
        if (!importGoldLimitByLevel(
                reinterpret_cast<unsigned int*>(m_moneyLimit)))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1eb7, 9, true, true)(
                "\t- Loading Money Limit per Level Fail");
        }
    }

    // Compound Item（bindsphere.etc；失败 TRACE 0x1ebf 后 ret0）。
    cMyTrace(__PRETTY_FUNCTION__, 0x1eb9, 9, false, true)("Success");
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ebb, 9, true, false)(
            "\t- Loading Compound Item ");
        // ORIG +0x2f6a..+0x2f95：new 结果直写全局后再读回作 this，
        // 不留具名局部（不留额外栈槽）。
        GlobalData::s_pCompoundBindCube =
            new ExtreamDungeon::CCompound_ExtreamDun_Item;
        if ((reinterpret_cast<ExtreamDungeon::CCompound_ExtreamDun_Item*>(
                 GlobalData::s_pCompoundBindCube)
                 ->LoadScript("Etc/", "bindsphere.etc")) != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1ebf, 9, true, true)(
                "\t- Fail load Compound Item Bind Cube Script File");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x1ec2, 9, false, true)("Success");
    }

    // ARAD Event Upgrade Jar（失败 TRACE 0x1ec9 ret0）。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ec5, 9, true, false)(
            "\t- Loading ARAD - Event Upgrade Jar ");
        // ORIG +0x30b4..+0x30df：`test %eax,%eax; sete %al` ⇒ 判定为
        // `== 0`（LoadScript 返回真=成功），非 `!= 0`。
        GlobalData::s_pEvent_UpGrade_Jar = new Event_Upgrade_Jar;
        if (GlobalData::s_pEvent_UpGrade_Jar->LoadScript(
                "", "Etc/EventUpgradeJar.etc") == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1ec9, 9, true, true)(
                "\t- Fail load Event.Upgrad.Jar Script File");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x1ecc, 9, false, true)("Success");
    }

    // Compound Avatar（nothrow new(0x88)；失败 ret0）。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ed9, 9, true, false)(
            "\t- Loading Compound Avatar ");
        // ORIG +0x321a..+0x3241：同样先写全局再读回。
        // [b28d-orig-nullfree] ORIG 报告 idx 2493-2503 无判空短路：写全局→读回→
        // 直接 LoadScript→`xor $1`（bool）。旧 `== 0 ||` 守卫系我方发明（EXTRA
        // test/je ×2）。
        GlobalData::s_pCompoundAvatar =
            new (std::nothrow) WongWork::CCompoundAvatar;
        if (!(reinterpret_cast<WongWork::CCompoundAvatar*>(
                  GlobalData::s_pCompoundAvatar)
                  ->LoadScript("Etc/", "CompoundAvatar.etc")))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1ee2, 9, true, true)(
                "\t- Loading Compound Avatar Fail\n");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x1ee5, 9, false, true)("Success");
    }

    // VendingMachine.lst（无条件）。
    importVendingMachineInfo("Etc/", "VendingMachine.lst", m_vendingMachine);

    // WorldMap/StageMap。
    if (!initWorldMapScript((char*)"WorldMap/", (char*)"WorldMap.lst"))
    {
        // [b28d-solo-CDataManager 真语义缺口修复] ORIG 0x0835b652-59：
        // `movl $0x8c31c07,(%esp); call puts`（串 "Load WorldMap.lst fail"）
        // ⇒ 失败路径有 puts（旧码缺，p167 MISSING puts ×1）。
        puts("Load WorldMap.lst fail");
        return 0;
    }
    initStageMapScript((char*)"StageMap/", (char*)"StageMap.lst");

    // Dungeons：new CDungeonList(0x14) + dimension 区清零 + load_list。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ef3, 9, true, false)(
            "\t- Loading Dungeons ");
        m_dungeonList = new CDungeonList;
        memset(m_dimensionInout, 0, 6);
        memset(m_originalDimensionInout, 0, 6);
        memset(m_limitInoutCount, 0, 3);
        if (!m_dungeonList->load_list())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1f05, 9, true, true)(
                "\t- Loading Dungeons Fail");
            return 0;
        }
    }

    // Tower Of Despair（失败 TRACE 0x1f0f 后 ret0）。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f08, 9, false, true)("Success");
        cMyTrace(__PRETTY_FUNCTION__, 0x1f0b, 9, true, false)(
            "\t- Loading Tower Of Despair System ");
        // ORIG +0x355d..+0x3588：写全局→读回→`!= 0` 判定。
        GlobalData::s_pTowerOfDespairMgr = new TowerOfDespairMgr;
        if ((reinterpret_cast<TowerOfDespairMgr*>(
                 GlobalData::s_pTowerOfDespairMgr)
                 ->LoadScript("Etc/", "TowerOfDespair.etc")) != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1f0f, 9, true, true)(
                "\t- Fail load TowerOfDespair System Script");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x1f12, 9, false, true)("Success");
    }

    // Chatting Emoticon：new(0x48) + lst + Load_list。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f16, 9, true, false)(
            "\t- Loading Chatting Emoticon ");
        m_emoticonList = new CChattingEmoticonList;
        if (!initChattingEmoticonScriptList((char*)"ChatEmoticon/",
                                            (char*)"ChatEmoticon.lst"))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1f1b, 9, true, true)(
                "\t- Emoticon List file error");
            return 0;
        }
        if (!m_emoticonList->Load_list())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1f20, 9, true, true)(
                "\t- Emoticon Script file fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x1f23, 9, false, true)("Success");
    }

    // PvP 地图基础数据 + Maps。
    // pvpMaps 亦为函数体作用域对象（ORIG 清理梯 +0x6e5a：先 pvpMaps 后
    // schedules），不套嵌套块。
    std::vector<PVPMAP_DATA> pvpMaps;
    if (!loadPvpMapBasicDataScript("Etc/pvpmapbasicdata.etc", &pvpMaps))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f29, 9, true, true)(
            "Load Etc/pvpmapbasicdata.etc fail");
        return 0;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x1f2d, 9, true, false)(
        "\t- Loading Maps ");
    m_mapList = new CMapList;
    if (!load_map_files(pvpMaps))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f33, 9, true, true)(
            "\t- Loading Maps Fail");
        return 0;
    }

    // Monsters。
    cMyTrace(__PRETTY_FUNCTION__, 0x1f36, 9, false, true)("Success");
    cMyTrace(__PRETTY_FUNCTION__, 0x1f38, 9, true, false)(
        "\t- Loading Monsters ");
    m_monsterManager = new CMonsterManager;
    if (!m_monsterManager->load_monsters())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f3e, 9, true, true)(
            "\t- Loading Monsters Fail");
        return 0;
    }

    // IndependentDrop 缓存清理（无条件）+ Skills。
    cMyTrace(__PRETTY_FUNCTION__, 0x1f41, 9, false, true)("Success");
    m_independentDrop.clear();
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f47, 9, true, false)(
            "\t- Loading Skills ");
        m_skillList = new CSkillList;
        if (!m_skillList->init())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1f4d, 9, true, true)(
                "\t- Loading Skills Fail");
            return 0;
        }
    }

    // Creature Damage + ActiveStatus（SDCActiveStatusSyncServer 栈局部）。
    cMyTrace(__PRETTY_FUNCTION__, 0x1f50, 9, false, true)("Success");
    cMyTrace(__PRETTY_FUNCTION__, 0x1f52, 9, true, false)(
        "\t- Loading Creatrue Damage ");
    if (!prepareServerDelayCheckDamage())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f55, 9, true, true)(
            "\t- Loading Creatrue Damage Fail");
        return 0;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x1f58, 9, false, true)("Success");

    // activeStatus 同样是函数体作用域对象（ORIG 清理梯 +0x6e32 段；槽
    // -0xf35 仅 1 字节 ⇒ 空类）。
    SDCActiveStatusSyncServer activeStatus;
    cMyTrace(__PRETTY_FUNCTION__, 0x1f5c, 9, true, false)(
        "\t- Loading ActiveStatus Damage ");
    if (!activeStatus.loadActiveStatusScript())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f5f, 9, true, true)(
            "\t- Loading ActiveStatus Damage Fail");
        return 0;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x1f62, 9, false, true)("Success");

    // ItemGroupName.tbl（返回非 0 = 失败 ret0）+ Items。
    // —— 以下局部按 ORIG 槽序声明（-0xf54 数组 → -0xf58/-0xf5c/-0xf60 三个
    // 迭代器 → -0x107c roulette → -0x1910 STEquipmentScript）：ORIG 清理梯
    // 证明 roulette 是函数体作用域对象，而数组/迭代器槽位比 roulette 更浅
    // ⇒ 源码在 roulette 之前声明、之后才赋值（老式「顶部声明」风格）。
    const char* randomOptionFiles[7];
    // ORIG +0x3100：结果落 errDT 槽（-0x3c，与 DeathTower 同一变量，本处不再
    // 置 0）→ `cmpl $0,-0x3c; setne %al; test %al,%al`。
    if ((errDT = importItmeGroupTable("Etc/", "ItemGroupName.tbl",
                                      getItemGroupNameTable())) != 0)
    {
        return 0;
    }
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f69, 9, true, false)(
            "\t- Loading Items ");
        m_itemList = new CItemList;
        if (!m_itemList->init())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1f6f, 9, true, true)(
                "\t- Loading items Fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x1f72, 9, false, true)("Success");
    }

    // Avatar Roulette（栈局部 roulette → import → new Server → Init →
    // InitAddCharacSkill，失败 LogManager ret0）。
    // roulette 为函数体作用域对象（ORIG 清理梯顶段 +0x6e0a），不套嵌套块。
    cMyTrace(__PRETTY_FUNCTION__, 0x1f8a, 9, true, false)(
        "\t- Loading Avatar Roulette ");
    AvatarRoulette roulette;
    if (!importAvatarRouletteList((char*)"Etc/avatar_roulette_jar.etc",
                                  roulette))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f90, 9, true, true)(
            "\t- Loading Avatar Roulette Fail");
        return 0;
    }
    GlobalData::s_pAvatarRoulette = new AvatarRouletteServer;
    if (GlobalData::s_pAvatarRoulette == 0)
    {
        return 0;
    }
    if (!(reinterpret_cast<AvatarRouletteServer*>(GlobalData::s_pAvatarRoulette)
              ->Init(&roulette)))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f9a, 9, true, true)(
            "\t- Initalizing Avatar Roulette Fail");
        return 0;
    }
    if (!(reinterpret_cast<AvatarRouletteServer*>(GlobalData::s_pAvatarRoulette)
              ->InitAddCharacSkill()))
    {
        LogManager::logFormat(
            // [P8S4 2026-09-11] §6.5 真错：手写串写成 `int CDataManager::init()`，
            // ORIG 该站点实参 = prett 符号（串 `bool CDataManager::init()`，size 0x1a
            // @0x08c343cb）⇒ 运行期日志函数名文本错误，改回 __PRETTY_FUNCTION__。
            1, "data_manager.cpp", __PRETTY_FUNCTION__, 0x1fa1,
            "[AVATAR ROULETTE] initalizing(InitAddCharacSkill) failed.");
        return 0;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x1fa6, 9, false, true)("Success");

    // RandomOption 七文件循环（任一 err!=0 → TRACE ret0）。
    // ORIG +0x3254..+0x3261：err 先置 0，再 7 条字符串指针存入局部数组
    //（槽 -0xf54..-0xf3c），然后才打 “Loading RandomOption”。
    errRO = 0;
    randomOptionFiles[0] = "Etc/RandomOption/OptionGrouping.etc";
    randomOptionFiles[1] = "Etc/RandomOption/OptionGroupSelection.etc";
    randomOptionFiles[2] = "Etc/RandomOption/OptionNumbering.etc";
    randomOptionFiles[3] = "Etc/RandomOption/OptionQuantity.etc";
    randomOptionFiles[4] = "Etc/RandomOption/PartSelection.etc";
    randomOptionFiles[5] = "Etc/RandomOption/RandomizedOptionOverall1.etc";
    randomOptionFiles[6] = "Etc/RandomOption/RandomizedOptionOverall2.etc";
    cMyTrace(__PRETTY_FUNCTION__, 0x1fb6, 9, true, false)(
        "\t- Loading RandomOption ");
    // [P310] ORIG 循环尾检 = `mov -S,%eax; cmp $0x6,%eax; setbe; test; jne`
    //（idx 3308-3313）——局部须 int + 显式 (unsigned int) 比较才发两段寄存器形
    //（4.4 对 unsigned 局部恒折 cmpl 直比；探针 /tmp/probe_loop.cpp loop2）。
    for (int i = 0; (unsigned int)i <= 6; ++i)
    {
        if ((errRO = importRandomOptionScript(m_randomOption, randomOptionFiles[i])) != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1fbb, 9, true, true)(
                "\t- Fail load RandomOption Script File name(%s), "
                "error_line(%d)\n", randomOptionFiles[i], errRO);
            return 0;
        }
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x1fbf, 9, false, true)("Success");

    // initRandomOptionItem + random_option_ref DB 同步。
    if (!initRandomOptionItem())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1fc3, 9, true, true)(
            "Fail load initRandomOptionItem");
        return 0;
    }
    if (!sync_script::GetInstanceSyncScript()->truncate_random_option_ref())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1fc9, 9, true, true)(
            "\t- Fail truncate_random_option_ref");
        return 0;
    }
    if (!sync_script::GetInstanceSyncScript()->insert_random_option_ref())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1fcf, 9, true, true)(
            "\t- Fail truncate_random_option_ref");
        return 0;
    }
    if (m_regeneration._0_Load_RegenerationRandomOptionItem() != 0)
    {
        return 0;
    }

    // WorldMapList + Quests + Characters + Disjoints。
    if (!m_worldMapList.load_list())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1fe1, 9, true, true)(
            "\t- Loading World Map List Fail");
        return 0;
    }
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1fe5, 9, true, false)(
            "\t- Loading Quests ");
        m_questList = new QuestList;
        if (!m_questList->load_list())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1fe9, 9, true, true)(
                "\t- Loading Quests Fail");
            return 0;
        }
    }
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1fec, 9, false, true)("Success");
        cMyTrace(__PRETTY_FUNCTION__, 0x1fee, 9, true, false)(
            "\t- Loading Characters ");
        m_characList = new CCharacList;
        if (!m_characList->init())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1ff2, 9, true, true)(
                "\t- Loading Characters Fail");
            return 0;
        }
    }
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1ff5, 9, false, true)("Success");
        cMyTrace(__PRETTY_FUNCTION__, 0x1ff7, 9, true, false)(
            "\t- Loading Disjoints ");
        m_disJoint = new DisJoint;
        if (!m_disJoint->LoadDisJoint())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1ffb, 9, true, true)(
                "\t- Loading Disjoints Fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x1ffe, 9, false, true)("Success");
    }

    // Advance Altar：StageMapList::loadList。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2001, 9, true, false)(
            "\t- Loading Advance Altar ");
        if (!m_stageMapList.loadList())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2004, 9, true, true)(
                "\t- Loading Advance Altar Fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x2007, 9, false, true)("Success");
    }

    // ItemGeneratorMgr / AutoMarket。
    if (m_itemGeneratorMgr == 0)
    {
        m_itemGeneratorMgr = new WongWork::CItemGeneratorMgr;
    }
    if (!m_itemGeneratorMgr->initialize())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2012, 9, true, true)(
            "\t- Loading Item Generator Manager Fail\n");
        return 0;
    }
    // autoMarket：ORIG 复用 errRO（槽 -0x30，本处不再置 0）。
    if (m_autoMarket == 0)
    {
        m_autoMarket = new CAutoMarketConditionsControl;
    }
    if ((errRO = m_autoMarket->LoadScript("Etc/ItemDropInfo_Control.etc")) != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x201a, 9, true, true)(
            "\t- LoadScript() error Etc/ItemDropInfo_Control.etc line(%d)",
            errRO);
        return 0;
    }

    // LevelExp / MobReward / PvpGrade / RewardSP。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2022, 9, true, false)(
            "\t- Init Level Exp ");
        // [R3a02 2026-09-12 SEMANTIC-DIFF ④ 复刻] ORIG 0x0835cecb 机器码 `eb 56`
        // = 出厂二进制热补（原 `74 56` je → `eb 56` jmp，位移完全一致）⇒
        // “Init Level Exp Fail” 块（0x0835cecd..0x0835cf1e，14 条）恒不可达
        // （Ghidra C 亦只见直调 `set_level_exp(this);` 无 if）。按用户裁定
        // 「完全复刻 ORIG 逻辑（即使是错误逻辑）」+ P337：保留块体（维持
        // cMyTrace 调用点与 ORIG 齐平），以恒 0 volatile 守卫使运行期不可达。
        // [R18a02 2026-09-13] 形态收敛：`&&` 形在本 GCC 下产 12 条（含 3 个 je 与
        //   `mov $1/%eax;jmp;mov $0,%eax;test;je` 物化段），嵌套 if 形仅 7 条且与
        //   ORIG 站点（`call;xor $1;test %al,%al;jmp`）逐段同序（探针 /tmp/verify357/
        //   R18a02/probe/p.cpp t1 vs t2）⇒ 按用户口径「我方多出的物化一律移除」改写。
        volatile bool deadLevelExpFail = false;
        if (!set_level_exp())
        {
        if (deadLevelExpFail)
        {
        cMyTrace(__PRETTY_FUNCTION__, 0x2025, 9, true, true)(
            "\t- Init Level Exp Fail");
        return 0;
        }
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x2028, 9, false, true)("Success");
        cMyTrace(__PRETTY_FUNCTION__, 0x202a, 9, true, false)(
            "\t- Init Mob Reward ");
        // 同上：ORIG 0x0835cfcb 机器码 `eb 56`（原 `74 56` je → `eb 56` jmp），
        // 失败块（0x0835cfcd..0x0835d01e，14 条）恒不可达；文案沿用 ORIG 的
        // 复制粘贴痕迹 “Init Level Exp Fail”（+0x4cd1）。
        // 按补丁后运行时可观察行为复刻（同 P337 恒 0 volatile 守卫）。
        volatile bool deadMobRewardFail = false;
        if (!set_mob_reward())
        {
        if (deadMobRewardFail)
        {
        cMyTrace(__PRETTY_FUNCTION__, 0x202d, 9, true, true)(
            "\t- Init Level Exp Fail");
        return 0;
        }
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x2030, 9, false, true)("Success");
        cMyTrace(__PRETTY_FUNCTION__, 0x2032, 9, true, false)(
            "\t- Init PVP Grade ");
        if (!set_pvp_grade())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2035, 9, true, true)(
                "\t- Init PVP Grade Fail\n");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x2038, 9, false, true)("Success");
        cMyTrace(__PRETTY_FUNCTION__, 0x203a, 9, true, false)(
            "\t- Init Reward SP ");
        if (!set_reward_sp())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x203d, 9, true, true)(
                "\t- Init Reward SP Fail\n");
            return 0;
        }
    }

    // Channel 数据/脚本。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2040, 9, false, true)("Success");
        cMyTrace(__PRETTY_FUNCTION__, 0x2044, 9, true, false)(
            "\t- Init Get Channel Data(channel_info.etc) ");
        if (!GlobalData::s_channel_proxy->SendCheckChannelScript())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2047, 9, true, true)(
                "\t- Init Get Channel Data(channel_info.etc) Fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x204a, 9, false, true)("Success");
        cMyTrace(__PRETTY_FUNCTION__, 0x204e, 9, true, false)(
            "\t- Init Channel Script ");
        // [P248 档①/P292] ORIG 调用点（报告 idx 3963-3966）：`call 88dc008
        // <importChannelScript>; xor $0x1,%eax; test %al,%al; je` = bool 取反形。
        // 探针 /tmp/probe_xor.cpp：int callee 的六种写法恒产 setcc 五条，唯
        // callee 返回 bool 时裸 `!f()` 逐条复现（旧 &1 等价改写多 2 条/处）。
        // callee 声明+定义已 bool 化，返回语句全为常量 0/1（0x088dc062 尾调
        // 透传形态不变）。
        if (!importChannelScript(&m_channelScript))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2058, 9, true, true)(
                "\t- Init Channel Script Fail");
            return 0;
        }
    }

    // ItemUpgrade / Separate / 3rdChronicle / Amplifier / Reseal。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x205b, 9, false, true)("Success");
        m_itemUpgrade = new WongWork::CItemUpgrade;
        cMyTrace(__PRETTY_FUNCTION__, 0x205e, 9, true, false)(
            "\t- Init Item Upgrade Info ");
        // [P248 档①/P292] ORIG idx 4023-4035：`mov 0x20(this),%eax; (%eax)+0x04
        // 取虚槽1; call *%edx; xor $0x1,%eax; test %al` = bool 取反形。虚槽目标
        // ORIG T 0x0854919a（CItemUpgrade）/0x0811eca8（Separate）均尾调透传
        // （无 movzbl ⇒ 整链同型 bool）；声明+定义已同步 bool 化。
        if (!m_itemUpgrade->LoadUpgradeScript("Etc/Upgrade.etc"))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2061, 9, true, true)(
                "\t- Init Item Upgrade Info Fail\n");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x2064, 9, false, true)("Success");
        m_itemUpgradeSeparate = new WongWork::CItemUpgrade_Separate;
        if (!m_itemUpgradeSeparate->LoadUpgradeScript("Etc/Upgrade_Separate.etc"))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x206a, 9, true, true)(
                "\t- Item Upgrade Separate Fail");
            return 0;
        }
        if (!m_itemUpgrade->Load3rdChronicleUpgradeScript("Etc/chronicle.etc"))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2072, 9, true, true)(
                "\t- Loading 3rd Chronice Upgrade Script Fail");
            return 0;
        }
        GlobalData::s_itemAmplifier_->loadScript("Etc/AmplifyItem.etc",
                                                 "Etc/AmplifyUpgrade.etc");
    }

    // Reseal / UnlimitChallenge / LevelRewardTable / LimitItemUsage。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2084, 9, true, false)(
            "\t- Init Reseal table ");
        if (!importResealTable("Etc/Reseal.etc", &m_resealTable))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x208c, 9, true, true)(
                "\t- Init Reseal table Fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x208f, 9, false, true)("Success");
        cMyTrace(__PRETTY_FUNCTION__, 0x2092, 9, true, false)(
            "\t- Init Unlimit Challenge Info ");
        if (!importUnlimitChallengeScript("Etc/UnlimitChallenge.etc",
                                          &m_unlimitChallenge))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2095, 9, true, true)(
                "\t- Init Unlimit Challenge Info Fail");
            return 0;
        }
        m_levelRewardTableMgr = CLevelRewardTableMgr::GetInstance();
        m_levelRewardTableMgr->SetChallengeInfo(m_unlimitChallenge);
        cMyTrace(__PRETTY_FUNCTION__, 0x209e, 9, false, true)("Success");
        if (!importLimitItemUsageInfo(
                (char*)"Etc/Chn_Server_LimititemUsageinfo.etc",
                &m_limitItemUsage))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x20a5, 9, true, true)(
                "\t- importLimitItemUsageInfo Fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x20a8, 9, true, true)(
            "\t- Import Limit Item Usage Info Success");
    }

    // DB Log Item。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x20b7, 9, true, true)(
            "\t- Import DB Log Item - ");
        if (!importDBLogItem((char*)"Etc/Chn_DBLogItem.etc", &m_dbLogItems))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x20be, 9, true, true)(
                "\t- Import DB Log Item  Fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x20c1, 9, true, true)(
            "\t- Import DB Log Item Success");
    }

    // Condition Event 两件套。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x20db, 9, true, false)(
            "\t- Import Condition Event Info ");
        if (!importConditionEventScript("Event/ConditionEvent.evt",
                                        &m_conditionEvent))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x20e2, 9, true, true)(
                "\t- Import Condition Event Info Fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x20e5, 9, false, true)("Success");
        cMyTrace(__PRETTY_FUNCTION__, 0x2102, 9, true, false)(
            "\t- Import Condition Event Script Dungeon ");
        if (!importConditionEventChkDungeonScript(
                "Event/ConditionEventChkDungeon.evt", &m_conditionDungeon))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2109, 9, true, true)(
                "\t- Import Condition Event Script Dungeon Fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x210c, 9, false, true)("Success");
    }

    // NPCRelation / EmotionTags / EquipGrade。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2111, 9, true, false)(
            "\t- Import NPC Relation Event Info Success");
        if (!importNPCRelationEventScript("Event/NPCRelationEvent.evt",
                                          &m_npcRelation))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2118, 9, true, true)(
                "\t- Import NPC Relation Event Info Fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x211b, 9, false, true)("Success");
        cMyTrace(__PRETTY_FUNCTION__, 0x211e, 9, true, false)(
            "\t- Import Emotion Type Tags Info ");
        if (!importEmotionTypeTagsScript("Etc/EmotionExpressionData.etc",
                                         &m_emotionTags))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2121, 9, true, true)(
                "\t- Import Emotion Type Tags Info Fail\n");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x2124, 9, false, true)("Success");
        cMyTrace(__PRETTY_FUNCTION__, 0x2126, 9, true, false)(
            "\t- Import Equipment Grade Sample Info ");
        if (!importEquipmentGradeSampleInfo("Etc/EquipmentGradeSample.etc",
                                            &m_equipGrade))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2129, 9, true, true)(
                "\t- Import Equipment Grade Sample Info Fail\n");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x212c, 9, false, true)("Success");
    }

    // Creature 脚本两步。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x215c, 9, true, false)(
            "\t- Import Creature Script");
        if (!initCreatureScript("Creature/", "Creature.lst", "ExpTable.tbl"))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x215f, 9, true, true)(
                "\t- Import Creature Creature.lst fail");
            return 0;
        }
        if (!user_creature::GetInstanceCreatureScriptMgr()
                  ->LoadCreatureScriptFile())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2165, 9, true, true)(
                "\t- Import Creature Script Fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x2168, 9, false, true)("Success");
    }

    // PvP single playable map（静态，无条件）+ NPC Script。
    CMapList::LoadPvPSinglePlayableMap((char*)"extern.sd");
    m_npcScriptList = new CNPCScriptList;
    m_npcDynamicInfo = new CNPCDynamicInfoManager;
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x217a, 9, true, false)(
            "\t- Import Npc Script ");
        if (m_npcScriptList->initScript((char*)"Npc/", (char*)"Npc.lst",
                                        (char*)"common.dat") != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x217d, 9, true, true)(
                "\t- Import Npc.lst Fail\n");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x2180, 9, false, true)("Success");
    }

    // ItemLimitEditionMgr（m_npcRelation.m_itemLimitEditionMgr）+ MCAP init。
    m_npcRelation.m_itemLimitEditionMgr = new CItemLimitEditionMgr;
    WongWork::CMCAPManager::init();

    // Equipment Part Set：trancate + import + g_equipmentPartSetMap_ 双层回灌。
    sync_script::GetInstanceSyncScript()->trancate_item_parts_set();
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x21ac, 9, true, false)(
            "\t- Import Equipment Part Set Info ");
        if (!importEquipmentPartSetInfo("Etc/EquipmentPartSet.etc",
                                        static_cast<ENUM_MODULE_TYPE>(3)))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x21af, 9, true, true)(
                "\t- Import Equipment Part Set Info Fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x21b3, 9, false, true)("Success");

        // 迭代器在 for-init 内声明并直接初始化（ORIG `begin` 的 sret 目标即
        // 迭代器槽 -0xf58/-0xf5c/-0xf60，无「临时+拷贝」⇒ 必须是声明初始化）。
        for (std::map<int, STEquipmentPartSetData>::iterator it =
                 g_equipmentPartSetMap_.begin();
             it != g_equipmentPartSetMap_.end(); ++it)
        {
            // [b28d-solo-CDataManager] ORIG 0x0835e4f0 `ptEv; add $0x4;
            // mov %eax,-0x20`（具名引用 data）+ 0x0835e4fe `add $0xc;
            // mov %eax,-0x1c`（具名 pieces 引用）——旧形每处字段访问重发
            // ptEv（p167 EXTRA operator-> ×5）。
            // [N93 封顶] ORIG 槽序 data(-0xf60 深)/pieces(-0xf58 浅) 与本侧相反；
            // 交换声明序实测反劣（5431/5432 且 658 行非槽差，物化顺序被破坏）
            // ⇒ 按 N93（块级对象槽向跨版本相反、具名引用不可翻）封顶。
            STEquipmentPartSetData& data = it->second;
            std::vector<STEquipmentPartSetPiece>& pieces = data.m_pieces;
            for (std::vector<STEquipmentPartSetPiece>::iterator pit =
                     pieces.begin();
                 pit != pieces.end(); ++pit)
            {
                sync_script::GetInstanceSyncScript()->insert_item_parts_set(
                    it->first, *pit);
            }
            // [b28d-orig-nullfree] ORIG 报告 idx 4810-4816 对 m_script 直接
            // `mov 0x8(%eax),%eax` 解引用、无任何判空（旧 `!= 0` 守卫系我方发明，
            // ELF diff +4 行 EXTRA）⇒ 去守卫，与 ORIG 同形无条件进内层 map 循环。
            for (std::map<int, EquipmentParameterInfo>::iterator eit =
                     data.m_script->m_paramMap678.begin();
                 eit != data.m_script->m_paramMap678.end(); ++eit)
            {
                STEquipmentScript es;
                es.m_strac = data.m_script->m_str668;
                // ORIG 0x0835e4f8：`call operator->; add $0x4; mov %eax,-0x20`
                // 具名指针物化后两处复用（+0xc deref/…）——具名引用引导 4.4
                // 同形物化（段②站点对表首个试点）。
                // [b28d-orig-form] ORIG idx 4830-4838 对 eit->second 的
                // `ptEv; add $4` 结果直接落 operator= 实参槽（无具名引用中转，
                // 旧 rParam 引用多发 `mov %eax,-S; mov -S,%eax` 保存/取回 ×2）。
                es.m_param1c0 = eit->second;
                // [V331b02 2026-09-12 修复·实参来源互换] ORIG 0x0835e613..0x0835e646：
                //   `lea -0xf60(=%eit); ptEv<EquipmentParameterInfo>; mov (%eax),%esi`
                //   与 `lea -0xf58(=%it); ptEv<STEquipmentPartSetData>; mov (%eax),%eax;
                //   mov %eax,%ebx`，随后 `mov %esi,0x10(%esp)`(=arg3=key) /
                //   `mov %ebx,0x8(%esp)`(=arg1=idx) ⇒ ORIG 的 idx 取 STEPSD key、
                //   key 取 EPI key（与旧形恰好相反，两实参数据串位）。
                sync_script::GetInstanceSyncScript()
                    ->insert_equiptment_item_to_db(
                        &es, (unsigned int)it->first, 2, eit->first);
            }
        }
    }

    // FairPvPStat + PvP SkillTree 四步 + initFairPVPSkillPoint。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x21ca, 9, true, false)(
            "\t- Import FairPvPStat ");
        if (!importPairPvPStatScript(m_fairPvPStat, "Etc/FairPvPStat.etc"))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x21cd, 9, true, true)(
                "\t- Fail load FairPvPStat Script File");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x21d1, 9, false, true)("Success");
        if (!initPvPSkillTreeScript((char*)"Etc/", (char*)"PvPSkillTree.lst"))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x21d7, 9, true, true)(
                "\t- Load PvPSkillTree.lst fail");
            return 0;
        }
        if (!m_pvpSkillTree.load_pvp_skill_tree_list())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x21dd, 9, true, true)(
                "\t- Load load_pvp_skill_tree_list fail");
            return 0;
        }
        if (!m_pvpSkillTree.importPairPvPSkillPointScript(
                "Etc/PvPSkillPoint.etc"))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x21e3, 9, true, true)(
                "\t- Load PairPvPSkillPointScript fail");
            return 0;
        }
        if (!initFairPVPSkillPoint())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x21eb, 9, true, true)(
                "\t- initFairPVPSkillPoint fail");
            return 0;
        }
    }

    // Ingame Event：new(0x5c) CEventScriptMng → m_eventScriptMng；导入 + init。
    m_liveServerInfo.m_eventScriptMng = new EventClassify::CEventScriptMng;
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x221d, 9, true, false)(
            "\t- Loading Ingame Event Script List ");
        // [b28d-orig-form] ORIG 三站点各自独立 `mov 0xb504(%eax)` 重读成员
        // （报告 idx 5059/5093-5099），无局部指针缓存 ⇒ 表达式直接写成员路径。
        if (!importInGameEventInfoList(
                static_cast<InGameEventInfoList*>(m_liveServerInfo.m_eventScriptMng),
                "Event/InGameEventList.evt"))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2224, 9, true, true)(
                "\t- Fail load ingame_event_script_list");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x2228, 9, false, true)("Success");
        if (!m_liveServerInfo.m_eventScriptMng->initScriptlist(
                *m_liveServerInfo.m_eventScriptMng))
        {
            return 0;
        }
    }

    // Event Avatar List / Event Etc。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2231, 9, true, false)(
            "\t- Loading Event Avatar List ");
        if (!importEventAvatarListScript(&m_eventAvatarList,
                                         "Event/EventAvatarList.evt"))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x223d, 9, true, true)(
                "\t- Fail Loading Event Avatar List");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x2240, 9, false, true)("Success");
        cMyTrace(__PRETTY_FUNCTION__, 0x2244, 9, true, false)(
            "\t- Loading Event Etc Info ");
        if (!importEventEtcScript(&m_eventEtc, "Event/EventEtc.evt"))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x224e, 9, true, true)(
                "\t- Fail Loading Event Etc Info");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x2252, 9, false, true)("Success");
    }

    // AvatarColorVariation。
    if (!AvatarVariation::importAvatarColorVariation(&m_avatarColor))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x227b, 9, true, true)(
            "\t- Loading importAvatarColorVariation  Fail");
        return 0;
    }

    // Mercenary System（ORIG 报告 idx 5231-5239：errMerc 复位(-0x2c) → 独立
    // store → `cmpl $0; je` **两段形**（无 setne，旧「单表达式」判断有误））。
    cMyTrace(__PRETTY_FUNCTION__, 0x229b, 9, true, false)(
        "\t- Loading Mercenary System ");
    errMerc = 0;
    errMerc = importMercenarySystemEtcScript("Etc/mercenary.etc",
                                             &m_mercenarySystem);
    if (errMerc != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x22a0, 9, true, true)(
            "\t- Loading Mercenary System Fail - %d line \n", errMerc);
        return 0;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x22a3, 9, false, true)("Success");

    NeedMaterialDataManager::getInstance();

    // PcBang / DimensionActivation。
    if (!importPcBangEquipmentRendar("Etc/PcBangRentalService.etc",
                                     &m_pcBangRentar))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2304, 9, true, true)(
            "\t- PcBangRentalService Fail!\n");
        return 0;
    }
    if (!importDimensionActivationData("Event/DimensionActivationEvent.evt",
                                       &m_dimensionAct))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x230b, 9, true, true)(
            "\t- DimensionActivationEvent Fail!\n");
        return 0;
    }

    // 租借系统就绪检查。
    if (!reinterpret_cast<InstanceRentalSystem*>(GlobalData::s_rentalSystem)
              ->Ready())
    {
        std::cerr << "[GlobalData::Init] The rental system is not ready!"
                  << std::endl;
        return 0;
    }

    // Delete Invalid Item Script（最后一步）。
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2331, 9, true, false)(
            "\t- Load Delete Invalid Item Script");
        if (!m_deleteInvalidItem.loadScript())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2334, 9, true, true)(
                "\t- Load Delete Invalid Item Script Fail");
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x2337, 9, false, true)("Success");
    }

    return 1;
}