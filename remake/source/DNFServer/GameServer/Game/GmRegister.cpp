// GmRegister.cpp — game_master::CGameMasterMgr 构造注册链完整还原。
// 对照 ORIG CGameMasterMgr::CGameMasterMgr() @0x082a8a28（size 0x18fb）：
//   senior = new CSeniorGameMaster(0)；junior = new CJuniorGameMaster(senior)；
//   随后按 ORIG 顺序 80 条 add(int,CCommand*) + 7 条 add(std::string,CCommand*)。
#include "GameMasterCmd.h"
#include <string>

namespace game_master
{

CGameMasterMgr::CGameMasterMgr()                        // 0x082a8a28
{
    m_senior = new CSeniorGameMaster(0);                // 链尾
    m_junior = new CJuniorGameMaster(m_senior);         // 分发入口

    // ---- 数字 id 注册（ORIG 顺序逐条一致）----
    m_cmdMap.add(0x01, new CMoveCharacCmd);             // "move"
    m_cmdMap.add(0x02, new CPvPStartCmd);
    m_cmdMap.add(0x06, new CCirculationStatisticCmd);
    m_cmdMap.add(0x07, new CAutoMarketConditionsControlCmd);
    m_cmdMap.add(0x08, new CItemDropRateCmd);
    m_cmdMap.add(0x09, new CCharacStatCmd);
    m_cmdMap.add(0x0a, new CSetTutorialFlagCmd);
    m_cmdMap.add(0x0b, new CViewTutorialFlagCmd);
    m_cmdMap.add(0x0c, new CResetTutorialFlagCmd);
    m_cmdMap.add(0x0d, new CCreateDisjointStoreCmd);
    m_cmdMap.add(0x0e, new CRequestDisjointCmd);
    m_cmdMap.add(0x0f, new CTeleportCmd);
    m_cmdMap.add(0x10, new CChangeExpertJobCmd);
    m_cmdMap.add(0x11, new CChangeExpertJobExpCmd);
    m_cmdMap.add(0x12, new CStartMacroCmd);
    m_cmdMap.add(0x13, new CStopMacroCmd);
    m_cmdMap.add(0x14, new CRunMacroCmd);
    m_cmdMap.add(0x16, new CTestDisjointCmd);
    m_cmdMap.add(0x17, new CAssaultAbuseUserCmd);
    m_cmdMap.add(0x18, new CPcroomPremiumCmd);
    m_cmdMap.add(0x1d, new CBloodMonsterListCmd);
    m_cmdMap.add(0x1e, new CVillageAttackedStartCmd);
    m_cmdMap.add(0x21, new CVillageAttackedStopCmd);
    m_cmdMap.add(0x20, new CVillageAttackedFightCmd);
    m_cmdMap.add(0x22, new CItemInfoCmd);
    m_cmdMap.add(0x1f, new CAddItemPollutionCmd);
    m_cmdMap.add(0x23, new CChangeNPCMaxDayCmd);
    m_cmdMap.add(0x24, new CChangeNPCFavorCmd);
    m_cmdMap.add(0x25, new CClearInventoryCmd);
    m_cmdMap.add(0x26, new CClearAvatarInvenCmd);
    m_cmdMap.add(0x27, new CReselectDailyQuestCmd);
    m_cmdMap.add(0x28, new CResetTrainingQuestCmd);
    m_cmdMap.add(0x2a, new CInvalidGold);
    m_cmdMap.add(0x2b, new CPowerWarStraightVictory);
    m_cmdMap.add(0x2c, new CDeleteGuild);
    m_cmdMap.add(0x29, new CEnduranceCmd);
    m_cmdMap.add(0x2d, new CChangeCharacName);
    m_cmdMap.add(0x2e, new CStartEvent);
    m_cmdMap.add(0x2f, new CEndEvent);
    m_cmdMap.add(0x30, new CSecretShopCheckLottery);
    m_cmdMap.add(0x31, new CSecretShopNpc);
    m_cmdMap.add(0x34, new CItemDropInDungeon);
    m_cmdMap.add(0x36, new CDeleteAccountCargo);
    m_cmdMap.add(0x37, new CLinkSystem);
    m_cmdMap.add(0x3a, new CGuildExpBookDelete);
    m_cmdMap.add(0x3c, new CUserPremiumDelete);
    m_cmdMap.add(0x3d, new CAddItemByMail);
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
    m_cmdMap.add(0x82, new CBingoGmTest);

    // ---- 名字注册（ORIG 字符串字面量地址实证）----
    m_cmdMap.add(std::string("bongfail"), new CSetMultiboxFailCnt);        // 0x8c1621f
    m_cmdMap.add(std::string("lv"), new CLevelUp);                          // 0x8c16228
    m_cmdMap.add(std::string("recipeprob"), new CRecipeSuccess);            // 0x8c1622b
    m_cmdMap.add(std::string("cash"), new CBillingRechargeCommand);         // 0x8c16236
    m_cmdMap.add(std::string("restrict"), new CMobileRestrictCommand);      // 0x8c1623b
    m_cmdMap.add(std::string("pcroomrewardreset"), new CPcroomRewardReset); // 0x8c16244
    m_cmdMap.add(std::string("heromission"), new CHeroMissionCommand);      // 0x8c16256
}

} // namespace game_master
