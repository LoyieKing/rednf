// b28d-solo-GmRegister 判词（verdict=AE，check_tu_routed.sh clean 口径；旧 N77 封顶作废）：
//   ① ORIG 的 119 条 value-init 前置清零（47 个 new 站点，逐 dword/word；P299/P318）：
//      删除 GameMasterCmd.h 中 47 个命令类的用户默认 ctor（改由编译器隐式合成，R2 已广播）
//      + 调用点写 `new T()`——GCC 4.4 仅在类**无用户提供 ctor** 时发射前置清零
//      （探针 /tmp/b28d_gmreg_probe/p1/p3/p4 实测）；另删 CPowerWarStraightVictory::m_pad[2]
//      （P26/P312：ORIG 该站点只发一条 movw，显式 pad 会被多清 2B）。
//   ② senior/junior 字段改基类指针 IGameMaster*（P211 异型化）：复现 ORIG
//      `mov %ebx,%eax; mov %eax,%edx`（派生→基赋值中转）与 `mov (%eax),%edx`（实参单段）。
//   验收：gap 0（1609=1609）；p167 调用点差集 CLEAR（318/318，0 missing/0 extra）；
//   our_insns 1489→1609；87 注册块 (new尺寸↔类↔id/串↔顺序) 与 ORIG 逐块全等。
CGameMasterMgr::CGameMasterMgr()                        // 0x082a8a28
{
    m_senior = new CSeniorGameMaster(0);                // 链尾
    m_junior = new CJuniorGameMaster(m_senior);         // 分发入口

    // ---- 数字 id 注册（ORIG 顺序逐条一致）----
    // b28d-solo-GmRegister：以下 47 个类在 ORIG 均带 `new T()` value-init 前置清零
    // （P299/P318；类侧已删用户默认 ctor，改由编译器隐式合成）⇒ 必须写带括号 new。
    m_cmdMap.add(0x01, new CMoveCharacCmd());           // "move"
    m_cmdMap.add(0x02, new CPvPStartCmd());
    m_cmdMap.add(0x06, new CCirculationStatisticCmd());
    m_cmdMap.add(0x07, new CAutoMarketConditionsControlCmd());
    m_cmdMap.add(0x08, new CItemDropRateCmd());
    m_cmdMap.add(0x09, new CCharacStatCmd());
    m_cmdMap.add(0x0a, new CSetTutorialFlagCmd());
    m_cmdMap.add(0x0b, new CViewTutorialFlagCmd());
    m_cmdMap.add(0x0c, new CResetTutorialFlagCmd());
    m_cmdMap.add(0x0d, new CCreateDisjointStoreCmd());
    m_cmdMap.add(0x0e, new CRequestDisjointCmd());
    m_cmdMap.add(0x0f, new CTeleportCmd());
    m_cmdMap.add(0x10, new CChangeExpertJobCmd());
    m_cmdMap.add(0x11, new CChangeExpertJobExpCmd());
    m_cmdMap.add(0x12, new CStartMacroCmd());
    m_cmdMap.add(0x13, new CStopMacroCmd());
    m_cmdMap.add(0x14, new CRunMacroCmd());
    m_cmdMap.add(0x16, new CTestDisjointCmd());
    m_cmdMap.add(0x17, new CAssaultAbuseUserCmd());
    m_cmdMap.add(0x18, new CPcroomPremiumCmd());
    m_cmdMap.add(0x1d, new CBloodMonsterListCmd());
    m_cmdMap.add(0x1e, new CVillageAttackedStartCmd());
    m_cmdMap.add(0x21, new CVillageAttackedStopCmd());
    m_cmdMap.add(0x20, new CVillageAttackedFightCmd());
    m_cmdMap.add(0x22, new CItemInfoCmd());
    m_cmdMap.add(0x1f, new CAddItemPollutionCmd());
    m_cmdMap.add(0x23, new CChangeNPCMaxDayCmd());
    m_cmdMap.add(0x24, new CChangeNPCFavorCmd());
    m_cmdMap.add(0x25, new CClearInventoryCmd());
    m_cmdMap.add(0x26, new CClearAvatarInvenCmd());
    m_cmdMap.add(0x27, new CReselectDailyQuestCmd());
    m_cmdMap.add(0x28, new CResetTrainingQuestCmd());
    m_cmdMap.add(0x2a, new CInvalidGold());
    m_cmdMap.add(0x2b, new CPowerWarStraightVictory());
    m_cmdMap.add(0x2c, new CDeleteGuild());
    m_cmdMap.add(0x29, new CEnduranceCmd());
    m_cmdMap.add(0x2d, new CChangeCharacName());
    m_cmdMap.add(0x2e, new CStartEvent());
    m_cmdMap.add(0x2f, new CEndEvent());
    m_cmdMap.add(0x30, new CSecretShopCheckLottery());
    m_cmdMap.add(0x31, new CSecretShopNpc());
    m_cmdMap.add(0x34, new CItemDropInDungeon());
    m_cmdMap.add(0x36, new CDeleteAccountCargo());
    m_cmdMap.add(0x37, new CLinkSystem());
    m_cmdMap.add(0x3a, new CGuildExpBookDelete());
    m_cmdMap.add(0x3c, new CUserPremiumDelete());
    m_cmdMap.add(0x3d, new CAddItemByMail());
    m_cmdMap.add(0x3f, new CAdjustCharacStat);
    m_cmdMap.add(0x41, new CSetGuildFund);
    m_cmdMap.add(0x42, new CGuildDebug);
    m_cmdMap.add(0x44, new CDropItem);
    m_cmdMap.add(0x46, new CGetCashInven);
    m_cmdMap.add(0x47, new CPickup);
    m_cmdMap.add(0x48, new CRefund);
    m_cmdMap.add(0x4b, new CMonsterDropTest);
    m_cmdMap.add(0x4c, new CBoosterTest);
    m_cmdMap.add(0x4d, new CRandomObjectTest);
    m_cmdMap.add(0x4e, new CBindCubeTest);
    m_cmdMap.add(0x52, new COneADayItemShop);
    m_cmdMap.add(0x79, new CCraneNeedMaterial);
    m_cmdMap.add(0x5d, new CTitleBookReset);
    m_cmdMap.add(0x5e, new CTitleBookAdjust);
    m_cmdMap.add(0x5f, new CTitleBookNoClear);
    m_cmdMap.add(0x60, new CAchievementReset);
    m_cmdMap.add(0x61, new CAchievementReport);
    m_cmdMap.add(0x62, new CAchievementClear);
    m_cmdMap.add(0x63, new CAchievementAllClear);
    m_cmdMap.add(0x64, new CAchievementNoClear);
    m_cmdMap.add(0x66, new CInGameEventSetDay);
    m_cmdMap.add(0x6d, new CChoiceItem);
    m_cmdMap.add(0x6f, new CIndependentDrop);
    m_cmdMap.add(0x70, new CUpgradeCardDebug);
    m_cmdMap.add(0x76, new NpcLimitBuyItem);
    m_cmdMap.add(0x77, new CIndependentDropByForce);
    m_cmdMap.add(0x78, new DungeonDifficulty);
    m_cmdMap.add(0x7d, new BlueMarbleSet);
    m_cmdMap.add(0x7e, new CTestTradeRestrict);
    m_cmdMap.add(0x7f, new CGrowhWeaponEvent);
    m_cmdMap.add(0x80, new CClearRewardTest);

    // ---- 名字/混合注册（ORIG 顺序：bongfail 先于 0x82，汇编 0x1469 区实证）----
    m_cmdMap.add(std::string("bongfail"), new CSetMultiboxFailCnt);        // 0x8c1621f
    m_cmdMap.add(0x82, new CBingoGmTest);
    m_cmdMap.add(std::string("lv"), new CLevelUp);                          // 0x8c16228
    m_cmdMap.add(std::string("recipeprob"), new CRecipeSuccess);            // 0x8c1622b
    m_cmdMap.add(std::string("cash"), new CBillingRechargeCommand);         // 0x8c16236
    m_cmdMap.add(std::string("restrict"), new CMobileRestrictCommand);      // 0x8c1623b
    m_cmdMap.add(std::string("pcroomrewardreset"), new CPcroomRewardReset); // 0x8c16244
    m_cmdMap.add(std::string("heromission"), new CHeroMissionCommand);      // 0x8c16256
}