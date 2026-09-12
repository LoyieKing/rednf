#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUserCharacInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [PoolA 2026-09-11 fresh 复核 · 清单 6 件（routed 11:55，本 TU 四数 251/391/1/3，
//   missing=1 为 static_init 口径 N123）] 逐件三态与归因：
//   · get_charac_10_level_section / getCurCharacEscaladeTutorialFlag /
//     setCurCharacTutorialFlag：**AE**（62/63、69/71、108/109，ndiff 0；帧差为
//     跨版本 outgoing 预留，N115）。判词既有「旧 NEAR 系 --no-compile 伪差」成立。
//   · WarAreaKill 0x086459c4：**DIFF** 59/60 ndiff2。唯一残差 = 目标索引 lea 时机：
//     ORIG 0x08645a41 `lea 0x380(%ecx),%edx`（在 `fadds 0x14(%ebp)` 之后）；
//     4.4 提前到源基址装载前（`mov 0xc(%ebp),%edx; lea 0x380(%edx),%ecx`）并少 1 条
//     lea（两式 fstps 目标同址）。本波复核：`+=` 逐语句展开会加具名槽（P0 反例），
//     未再引入改动 ⇒ 维持 N（P75/N88 实参装载时刻族）。
//   · calc_disp_help_abuse_ratio 0x08658a4e：**NEAR** 68/68 ndiff8。残差 = 深槽区
//     （fildll 临时 -0x38/-0x34、cw -0x3a/-0x3c、除数 -0x4c）相对 ORIG
//     （-0x30/-0x2c、-0x32/-0x34、-0x3c）整体深 8，帧 0x68↔0x58 ⇒ N115/N95 维持。
//   · GetContinuousItemInfo 0x08677f24：**DIFF** 100/104 ndiff10。残差 = 4.4 把
//     `(index+0x460)` CSE 到 %esi 跨 call 保活（多发 push %esi）且 disp 0x10 折成
//     `lea 0x2(%esi),%edx`；ORIG 每处独立 `mov -0x18(%ebp),%ebx; …call; lea
//     0x460(%ebx),%ecx; mov %edx,0x10(%eax,%ecx,8)`（0x08677f92..0x08677f9d）
//     ⇒ N82附/N4（4.4 地址 CSE + disp/scale 折叠定则）。
//   ⇒ 6 件与既有判词一致；本轮零代码改动（仅登记）。
// ============================================================================

#include "CUserCharacInfo.h"

#include <string.h>

#include <algorithm>

#include "CUser.h"
#include "CInventory.h"
#include "CExpertJob.h"   // expert_job::CExpertJob 完整类型（IsBusyDisjointStore 用 GetType）
#include "ServerParameterScript.h"   // IncFatigueBatteryCharging/DieCurCharac 经 CDataManager+0x68 实例
#include "GameWorld.h"

// _Charac_info 布局视图（CharacInfoFields，0x14d1）见 CUserCharacInfo.h；
// 此处仅保留访问别名。ORIG 经完整类型直接成员寻址，本 TU 以同布局视图等价。
// ORIG 全局（0x9370f5c）：`.data` 初值 raw = 9c000000（156），全仓 0 个写点
// ⇒ 静态初值是唯一来源（我方原为 0/B，22+ 读取点全部按 0 计算）。
int DEFAULT_MAX_FATIGUE = 156;  // 原 GameStubs.cpp 定义迁移

// 具型视图直读（CUserCharacInfo.h 匿名 union m_selView）：值 cast 触发 4.4
// 二段拷贝（探针 /tmp/bl10/p2.cpp getE vs getF），直读复现 ORIG 单段装载。
#define CUR (m_selView)

// ORIG 平坦窗口视图（P1 组件视图：pad 前置使成员寻址折叠为单条 disp/scale 指令，
// 与具名字段等偏移；ORIG 源码即以“基址+魔数下标”形态书写，直接成员下标会丢
// 常量项使指令序列回退）：
//   WarAreaKill 0x08645a3a `flds 0xe(%edx,%ebx,4)`，ebx=index+0x380 ⇔
//     &m_warAreaKill[index]（m_warAreaKill[5]@+0xe0e = +0xe + 0x380*4）；
//   GetContinuousItemInfo 0x08677f97 `mov %edx,0x10(%eax,%ecx,8)`，
//     ecx=index+0x460 ⇔ cooltime 条目 @SIG_LOAD_ETC +0x2310 + 8i；
//   0x08678031 `mov %edx,0x8(%eax,%ecx,8)`，ecx=index+0x494 ⇔ effect 条目
//     @+0x24a8 + 8i。
#pragma pack(push, 1)
struct WarAreaKillWindow
{
    char m_pad0E[0xe];
    float m_warAreaKillFlat[0x385];   // [0x380+i] = +0xe0e + 4i
};
struct LoadEtcCoolItemWindow
{
    char m_pad10[0x10];
    ContinuousItemInfo m_entries[0x494];  // [0x460+i] = +0x2310 + 8i
};
struct LoadEtcEffectItemWindow
{
    char m_pad08[0x8];
    ContinuousItemInfo m_entries[0x4C8];  // [0x494+i] = +0x24a8 + 8i
};
// [b28d-solo-CUserCharacInfo N107/P106] m_inven 真实类型视图：CharacInfoFields 内
// `char m_inven[]` 占位使取址退化为数组加法（4.4 恒 `add $0xda`）；经真实
// CInventory 成员取址复现 ORIG `lea 0xda(%edx),%ecx`。pack(1) 保持 +0xda。
struct CharacInvenView
{
    char m_pad0[0xda];
    CInventory m_inven;    // +0xda（CInventory 内嵌）
};
#pragma pack(pop)
static_assert(offsetof(WarAreaKillWindow, m_warAreaKillFlat) == 0xe,
              "war area kill window base drift (ORIG flds 0xe(%edx,%ebx,4))");
static_assert(offsetof(LoadEtcCoolItemWindow, m_entries) == 0x10,
              "cooltime flat base drift (ORIG mov %edx,0x10(%eax,%ecx,8))");
static_assert(offsetof(LoadEtcEffectItemWindow, m_entries) == 0x8,
              "effect flat base drift (ORIG mov %edx,0x8(%eax,%ecx,8))");
static_assert(offsetof(CharacInvenView, m_inven) == 0xda,
              "inven view base drift (ORIG lea 0xda(%edx),%ecx)");

#include "CUserCharacInfo/dtor_CUserCharacInfo.inc"


#include "CUserCharacInfo/useWinPoint.inc"


#include "CUserCharacInfo/updateLastItemLimitEditionBuyTime.inc"



#include "CUserCharacInfo/subtractCurCharacExp.inc"



#include "CUserCharacInfo/subUltimateInoutValue.inc"


#include "CUserCharacInfo/subDemensionInoutValue.inc"


#include "CUserCharacInfo/set_uppermember_exp_level.inc"



#include "CUserCharacInfo/set_member_pay_tex_money_to_upper.inc"



#include "CUserCharacInfo/set_member_pay_tex_fatigue_to_upper.inc"



#include "CUserCharacInfo/set_level_before_dungeon.inc"



#include "CUserCharacInfo/set_guild_exp.inc"



#include "CUserCharacInfo/set_guild_create_right.inc"



#include "CUserCharacInfo/set_growth_power_reward.inc"



#include "CUserCharacInfo/set_growth_power_exp_reward_right.inc"



#include "CUserCharacInfo/set_connect_upper_member.inc"



#include "CUserCharacInfo/set_charac_visible_values.inc"



#include "CUserCharacInfo/set_charac_visible_teleport_.inc"



#include "CUserCharacInfo/set_charac_visible_grow_avatar.inc"



#include "CUserCharacInfo/set_charac_seria_buf_bonus_exp.inc"



#include "CUserCharacInfo/set_charac_party_bonus_exp.inc"



#include "CUserCharacInfo/set_charac_memberkey.inc"



#include "CUserCharacInfo/set_charac_member_bonus_exp.inc"



#include "CUserCharacInfo/set_charac_guildkey.inc"



#include "CUserCharacInfo/set_charac_fatigue_buf_bonus_exp.inc"



#include "CUserCharacInfo/set_charac_createtime.inc"


#include "CUserCharacInfo/set_aura_avatar_option_value.inc"



#include "CUserCharacInfo/set_aura_avatar_option.inc"




#include "CUserCharacInfo/setWeekendBonusFatigue.inc"



#include "CUserCharacInfo/setWeekendBonusExp.inc"



#include "CUserCharacInfo/setUserEventCharacterFlag.inc"

#include "CUserCharacInfo/setUpgradeSeparateTrueFlag.inc"



#include "CUserCharacInfo/setUpgradeSeparateFalseFlag.inc"



#include "CUserCharacInfo/setUltimateInoutValue.inc"


#include "CUserCharacInfo/setTagCharac.inc"

#include "CUserCharacInfo/setStraightVictories.inc"



#include "CUserCharacInfo/setSeriaRoomAniDecoInfo.inc"




#include "CUserCharacInfo/setSaveCharacOption.inc"


#include "CUserCharacInfo/setPremiumBonusExp.inc"



#include "CUserCharacInfo/setPowerWarHP.inc"



#include "CUserCharacInfo/setMemberDungeonFlag.inc"



#include "CUserCharacInfo/setLinkCharacSlotIndex.inc"



#include "CUserCharacInfo/setGuildMemberGrade.inc"



#include "CUserCharacInfo/setGuildCreateFlag.inc"



#include "CUserCharacInfo/isDisguiseCharac.inc"



#include "CUserCharacInfo/setDisguiseCharac.inc"



#include "CUserCharacInfo/setDisguiseKind.inc"



#include "CUserCharacInfo/setDisguiseIndex.inc"



#include "CUserCharacInfo/setDemensionInoutValue.inc"


#include "CUserCharacInfo/setCurrCharacJoinPower.inc"





#include "CUserCharacInfo/setCurrCharacBlackCount.inc"



#include "CUserCharacInfo/setCurUseBringUpMaterialTotal.inc"



#include "CUserCharacInfo/setCurEquipslotFlag.inc"



#include "CUserCharacInfo/setCurCharacVill.inc"




#include "CUserCharacInfo/setCurCharacTutorialFlag.inc"


#include "CUserCharacInfo/setCurCharacTradeGoldDaily.inc"


#include "CUserCharacInfo/setCurCharacTagCharacNo.inc"



#include "CUserCharacInfo/setCurCharacStrikerSkillIndex.inc"



#include "CUserCharacInfo/setCurCharacStamina.inc"




#include "CUserCharacInfo/setCurCharacPremiumFatigue.inc"



#include "CUserCharacInfo/setCurCharacMaxPremiumFatigue.inc"



#include "CUserCharacInfo/setCurCharacLevel.inc"



#include "CUserCharacInfo/setCurCharacLastPlayTickPowerWar.inc"




#include "CUserCharacInfo/setCurCharacLastPlayTick.inc"




#include "CUserCharacInfo/setCurCharacLastPlayDungeonIndex.inc"



#include "CUserCharacInfo/setCurCharacHelpAbuseComputedRatio.inc"



#include "CUserCharacInfo/setCurCharacGuildSecede.inc"


#include "CUserCharacInfo/setCurCharacFatigueGrownUpBuff.inc"





#include "CUserCharacInfo/setCurCharacFatigue.inc"



#include "CUserCharacInfo/setCurCharacExp.inc"



#include "CUserCharacInfo/setCurCharacEventCharacterGrowtype.inc"




#include "CUserCharacInfo/setCurCharacBackupStamina.inc"



#include "CUserCharacInfo/setCurCharac.inc"


#include "CUserCharacInfo/setCurCharGrowthType.inc"




#include "CUserCharacInfo/setCurCharChangingGrowthType.inc"




#include "CUserCharacInfo/setCurChannelEquipslotSwitch.inc"



#include "CUserCharacInfo/setConctractOfCubePremiumInfo.inc"


#include "CUserCharacInfo/setCharacViewState.inc"



#include "CUserCharacInfo/setCharacOption.inc"





#include "CUserCharacInfo/setCharacLinkBonusGold.inc"



#include "CUserCharacInfo/setCharacLinkBonusExp.inc"



#include "CUserCharacInfo/setBringUpRewardFlag.inc"



#include "CUserCharacInfo/setBlueMarbleEnterCount.inc"



#include "CUserCharacInfo/setBloodBestRecord.inc"



#include "CUserCharacInfo/setBloodBestRecordUpdate.inc"



#include "CUserCharacInfo/reset_level_before_dungeon.inc"



#include "CUserCharacInfo/reset_is_end_lethe_at_login.inc"



#include "CUserCharacInfo/reset_guild_today_exp.inc"



#include "CUserCharacInfo/reset_guild_temp_exp.inc"



#include "CUserCharacInfo/reset_guild_exp.inc"



#include "CUserCharacInfo/resetWeekendBonusExp.inc"



#include "CUserCharacInfo/resetTempGuildNameAddr.inc"



#include "CUserCharacInfo/resetStraightVictories.inc"



#include "CUserCharacInfo/resetPremiumBonusExp.inc"



#include "CUserCharacInfo/resetGuildCreateFlag.inc"



#include "CUserCharacInfo/resetCurUseBringUpMaterialCount.inc"



#include "CUserCharacInfo/resetCurCharacUsedFatigue.inc"



#include "CUserCharacInfo/resetCurCharacSchoolPoint.inc"



#include "CUserCharacInfo/resetCurCharacOverEquipFlag.inc"




#include "CUserCharacInfo/resetCurCharacMemberBonusFatigue.inc"





#include "CUserCharacInfo/resetCharacTutorialFlag.inc"



#include "CUserCharacInfo/resetCharacFatigueGrownUpBuff.inc"



#include "CUserCharacInfo/resetCharGuildInfo.inc"



#include "CUserCharacInfo/resetBlueMarbleUserDeadCount.inc"



#include "CUserCharacInfo/resetBlueMarbleEnterCount.inc"



#include "CUserCharacInfo/needCurCharacCheckOverEquip.inc"



#include "CUserCharacInfo/is_growth_power_exp_reward_right.inc"


#include "CUserCharacInfo/is_connect_upper_member.inc"





#include "CUserCharacInfo/isSetGuildCreateFlag.inc"




#include "CUserCharacInfo/isSavedPremiumCubeInfo.inc"



#include "CUserCharacInfo/isSaveSkill.inc"

#include "CUserCharacInfo/isSaveQuest.inc"

#include "CUserCharacInfo/isSavePvP.inc"

#include "CUserCharacInfo/isSaveInven.inc"

#include "CUserCharacInfo/isSaveDungeonClear.inc"

#include "CUserCharacInfo/isSaveCharacOption.inc"

#include "CUserCharacInfo/isSaveCharacInfo.inc"

#include "CUserCharacInfo/isSaveCharacEtc.inc"

#include "CUserCharacInfo/isSaveCargo.inc"

#include "CUserCharacInfo/isSaveBattleRecord.inc"

#include "CUserCharacInfo/isJoinPowerWar.inc"


#include "CUserCharacInfo/getPowerWarHP.inc"


#include "CUserCharacInfo/isExistItem.inc"




#include "CUserCharacInfo/isBloodBestRecordUpdate.inc"


#include "CUserCharacInfo/initContractOfCubePremiumInfo.inc"


#include "CUserCharacInfo/increaseBlueMarbleUserDeadCount.inc"



#include "CUserCharacInfo/increaseBlueMarbleEnterCount.inc"



#include "CUserCharacInfo/incStraightVictories.inc"



#include "CUserCharacInfo/incCurUseBringUpMaterialCount.inc"



#include "CUserCharacInfo/incCurCharacUsedFatigue.inc"



#include "CUserCharacInfo/incCurCharacMemberBonusFatigue.inc"




#include "CUserCharacInfo/incCurCharacLevel.inc"



#include "CUserCharacInfo/get_uppermember_exp_level.inc"


#include "CUserCharacInfo/get_pvp_result_oldexp_point.inc"

#include "CUserCharacInfo/get_pvp_result_exp_point.inc"

#include "CUserCharacInfo/get_pvp_grade.inc"

#include "CUserCharacInfo/get_member_pay_tex_money_to_upper.inc"


#include "CUserCharacInfo/get_member_pay_tex_fatigue_to_upper.inc"


#include "CUserCharacInfo/get_guild_today_exp.inc"


#include "CUserCharacInfo/get_guild_temp_exp.inc"


#include "CUserCharacInfo/get_guild_exp.inc"


#include "CUserCharacInfo/get_guild_create_right.inc"


#include "CUserCharacInfo/get_growth_power_reward.inc"


#include "CUserCharacInfo/get_connect_upper_member.inc"


#include "CUserCharacInfo/get_charac_visible_values.inc"


#include "CUserCharacInfo/get_charac_visible_grow_avatar.inc"



#include "CUserCharacInfo/get_charac_seria_buf_bonus_exp.inc"



#include "CUserCharacInfo/get_charac_party_bonus_exp.inc"


#include "CUserCharacInfo/get_charac_memberkey.inc"


#include "CUserCharacInfo/get_charac_member_bonus_exp.inc"


#include "CUserCharacInfo/get_charac_level_before_dungeon.inc"



#include "CUserCharacInfo/get_charac_job.inc"


#include "CUserCharacInfo/get_charac_guildkey.inc"


#include "CUserCharacInfo/get_charac_fatigue_buf_bonus_exp.inc"


#include "CUserCharacInfo/get_charac_exp.inc"


#include "CUserCharacInfo/get_charac_10_level_section.inc"


#include "CUserCharacInfo/getWeekendBonusExp.inc"


#include "CUserCharacInfo/getUserEventCharacterFlag.inc"

#include "CUserCharacInfo/getUpgradeSeparateTrueFlag.inc"


#include "CUserCharacInfo/getUpgradeSeparateFlaseFlag.inc"


#include "CUserCharacInfo/getUltimateInoutValue.inc"


#include "CUserCharacInfo/getTagCharacW.inc"

#include "CUserCharacInfo/getTagCharacSkillW.inc"


#include "CUserCharacInfo/getTagCharacSkillR.inc"


#include "CUserCharacInfo/getTagCharacR.inc"

#include "CUserCharacInfo/getTagCharacInvenW.inc"


#include "CUserCharacInfo/getTagCharacInvenR.inc"


#include "CUserCharacInfo/getTagCharacCargoW.inc"


#include "CUserCharacInfo/getTagCharacCargoR.inc"


#include "CUserCharacInfo/getTagCharacAddInfoR.inc"


#include "CUserCharacInfo/getStraightVictories.inc"


#include "CUserCharacInfo/getSkillCommand.inc"


#include "CUserCharacInfo/getSeriaRoomAniDecoInfo.inc"


#include "CUserCharacInfo/getSeparatedTextUnits.inc"


#include "CUserCharacInfo/getRequestRenameState.inc"


#include "CUserCharacInfo/getPremiumBonusExp.inc"


#include "CUserCharacInfo/getPVPResultW.inc"


#include "CUserCharacInfo/getPVPResultRefW.inc"

#include "CUserCharacInfo/getPVPResultRefR.inc"


#include "CUserCharacInfo/getPVPResultR.inc"


#include "CUserCharacInfo/getMemberDungeonFlag.inc"



#include "CUserCharacInfo/getLinkCharacSlotIndex.inc"



#include "CUserCharacInfo/getLastItemLimitEditionBuyTime.inc"


#include "CUserCharacInfo/getGuildMemberGrade.inc"


#include "CUserCharacInfo/getDungeonClearRefW.inc"


#include "CUserCharacInfo/getDungeonClearRefR.inc"

#include "CUserCharacInfo/getDisguiseKind.inc"


#include "CUserCharacInfo/getDisguiseIndex.inc"


#include "CUserCharacInfo/getDemensionInoutValue.inc"


#include "CUserCharacInfo/getCurrCharacBlackCount.inc"


#include "CUserCharacInfo/getCurUseBringUpMaterialTotal.inc"


#include "CUserCharacInfo/getCurUseBringUpMaterialCount.inc"


#include "CUserCharacInfo/getCurExpandEquipslotSwitch.inc"



#include "CUserCharacInfo/getCurEquipslotFlag.inc"



#include "CUserCharacInfo/getCurCharacW.inc"

#include "CUserCharacInfo/getCurCharacVillPrev.inc"


#include "CUserCharacInfo/getCurCharacVill4Save.inc"


#include "CUserCharacInfo/getCurCharacTutorialFlag.inc"


#include "CUserCharacInfo/getCurCharacTradeGoldTotalBillion.inc"


#include "CUserCharacInfo/getCurCharacTradeGoldTotal.inc"


#include "CUserCharacInfo/getCurCharacTradeGoldDaily.inc"


#include "CUserCharacInfo/getCurCharacTagCharacNo.inc"


#include "CUserCharacInfo/getCurCharacStrikerSkillIndex.inc"


#include "CUserCharacInfo/getCurCharacStamina.inc"


#include "CUserCharacInfo/getCurCharacSkillW.inc"


#include "CUserCharacInfo/getCurCharacSkillR.inc"


#include "CUserCharacInfo/getCurCharacSchoolPoint.inc"


#include "CUserCharacInfo/getCurCharacR.inc"

#include "CUserCharacInfo/getCurCharacPremiumFatigue.inc"


#include "CUserCharacInfo/getCurCharacPayCoin.inc"




#include "CUserCharacInfo/getCurCharacNo.inc"


#include "CUserCharacInfo/getCurCharacMoney.inc"




#include "CUserCharacInfo/getCurCharacMemberBonusFatigue.inc"


#include "CUserCharacInfo/getCurCharacMaxPremiumFatigue.inc"


#include "CUserCharacInfo/getCurCharacMaxFatigue.inc"


#include "CUserCharacInfo/getCurCharacLastPlayTickPowerWar.inc"


#include "CUserCharacInfo/getCurCharacLastPlayTick.inc"


#include "CUserCharacInfo/getCurCharacLastPlayDungeonIndex.inc"


#include "CUserCharacInfo/getCurCharacInvenW.inc"


#include "CUserCharacInfo/getCurCharacInvenRefW.inc"


#include "CUserCharacInfo/getCurCharacInvenRefR.inc"


#include "CUserCharacInfo/getCurCharacInvenR.inc"


#include "CUserCharacInfo/getCurCharacHelpAbuseComputedRatio.inc"


#include "CUserCharacInfo/getCurCharacGuildSecede.inc"


#include "CUserCharacInfo/getCurCharacGrowType.inc"


#include "CUserCharacInfo/getCurCharacFatigueGrownUpBuff.inc"


#include "CUserCharacInfo/getCurCharacFatigue.inc"


#include "CUserCharacInfo/getCurCharacExp.inc"


#include "CUserCharacInfo/getCurCharacEventCoin.inc"




#include "CUserCharacInfo/getCurCharacEventCharacterGrowtype.inc"


#include "CUserCharacInfo/getCurCharacEscaladeTutorialFlag.inc"


#include "CUserCharacInfo/getCurCharacEscaladeTutorialFlag_1.inc"


#include "CUserCharacInfo/getCurCharacCreateTime.inc"


#include "CUserCharacInfo/getCurCharacCoin.inc"




#include "CUserCharacInfo/getCurCharacCargoW.inc"


#include "CUserCharacInfo/getCurCharacCargoR.inc"


#include "CUserCharacInfo/getCurCharacAddInfoW.inc"


#include "CUserCharacInfo/getCurCharacAddInfoRefW.inc"


#include "CUserCharacInfo/getCurCharacAddInfoRefR.inc"


#include "CUserCharacInfo/getCurCharacAddInfoR.inc"


#include "CUserCharacInfo/getCurCharSecondGrowType.inc"


#include "CUserCharacInfo/getCurCharFirstGrowType.inc"


#include "CUserCharacInfo/getCurChannelEquipslotSwitch.inc"



#include "CUserCharacInfo/getCreateGuildName.inc"


#include "CUserCharacInfo/getCreateGuildAddr.inc"


#include "CUserCharacInfo/getContractOfCubePremiumInfo.inc"



#include "CUserCharacInfo/getCharacLinkType.inc"


#include "CUserCharacInfo/getCharacLinkBonusGold.inc"


#include "CUserCharacInfo/getCharacLinkBonusExp.inc"


#include "CUserCharacInfo/getBringUpRewardFlag.inc"


#include "CUserCharacInfo/getBlueMarbleUserDeadCount.inc"


#include "CUserCharacInfo/getBlueMarbleEnterCount.inc"


#include "CUserCharacInfo/getBloodBestTimeRecord.inc"


#include "CUserCharacInfo/getBloodBestRoundRecord.inc"


#include "CUserCharacInfo/getBattleRecordRefW.inc"


#include "CUserCharacInfo/getBattleRecordRefR.inc"


#include "CUserCharacInfo/enableSaveSkill.inc"

#include "CUserCharacInfo/enableSaveQuest.inc"

#include "CUserCharacInfo/enableSavePvP.inc"

#include "CUserCharacInfo/enableSaveInven.inc"

#include "CUserCharacInfo/enableSaveDungeonClear.inc"

#include "CUserCharacInfo/enableSaveCharacEtc.inc"

#include "CUserCharacInfo/enableSaveCargo.inc"

#include "CUserCharacInfo/enableSaveBattleRecord.inc"

#include "CUserCharacInfo/disableSavedPremiumCubeInfo.inc"




#include "CUserCharacInfo/disableSaveSkill.inc"

#include "CUserCharacInfo/disableSaveQuest.inc"

#include "CUserCharacInfo/disableSavePvP.inc"

#include "CUserCharacInfo/disableSaveInven.inc"

#include "CUserCharacInfo/disableSaveDungeonClear.inc"

#include "CUserCharacInfo/disableSaveCharacStat.inc"

#include "CUserCharacInfo/disableSaveCharacManageInfo.inc"

#include "CUserCharacInfo/disableSaveCharacInfo.inc"

#include "CUserCharacInfo/disableSaveCharacEtc.inc"

#include "CUserCharacInfo/disableSaveCargo.inc"

#include "CUserCharacInfo/disableSaveBattleRecord.inc"

#include "CUserCharacInfo/decCurCharacLevel.inc"



#include "CUserCharacInfo/decCurCharacFatigueGrownUpBuff.inc"



#include "CUserCharacInfo/checkBonusPoint.inc"




#include "CUserCharacInfo/resetBonusPoint.inc"



#include "CUserCharacInfo/getWpBonusPoint.inc"





#include "CUserCharacInfo/getExpBonusPoint.inc"





#include "CUserCharacInfo/get_level_up_exp.inc"


#include "CUserCharacInfo/getCurCharacDungeonHelpAbuseRatio.inc"


#include "CUserCharacInfo/getCurCharacDungeonHelpAbuseExp.inc"


#include "CUserCharacInfo/calc_disp_help_abuse_ratio.inc"


#include "CUserCharacInfo/calcHelpAbuseRatioBeforeLevelUp.inc"



#include "CUserCharacInfo/calcHelpAbuseRatio.inc"





#include "CUserCharacInfo/calcCurCharacSchoolPoint.inc"



#include "CUserCharacInfo/getCurCharacUsedFatigue.inc"


#include "CUserCharacInfo/add_member_pay_tex_money_to_upper.inc"



#include "CUserCharacInfo/add_member_pay_tex_fatigue_to_upper.inc"



#include "CUserCharacInfo/add_guild_today_exp.inc"



#include "CUserCharacInfo/add_guild_temp_exp.inc"



#include "CUserCharacInfo/add_guild_exp.inc"



#include "CUserCharacInfo/addCurcharacStamina.inc"



#include "CUserCharacInfo/addCurCharacExp.inc"



#include "CUserCharacInfo/WarAreaKill.inc"


#include "CUserCharacInfo/VillageAttack_NotDBUpdate.inc"



#include "CUserCharacInfo/VillageAttack_DBUpdate.inc"



#include "CUserCharacInfo/UseFinishPoint.inc"


#include "CUserCharacInfo/SubCurCharacChaosPoint.inc"



#include "CUserCharacInfo/SetVisible.inc"


#include "CUserCharacInfo/SetSaveDemensionInoutValue.inc"





#include "CUserCharacInfo/SetSaveBloodInoutValue.inc"





#include "CUserCharacInfo/SetReliablePerson.inc"


#include "CUserCharacInfo/SetProperDungeonUser.inc"



#include "CUserCharacInfo/SetPowerWarProcessInfoLastUpdateTime.inc"



#include "CUserCharacInfo/SetPowerWarLastPlayTime.inc"



#include "CUserCharacInfo/SetOverTradeGoldCount.inc"


#include "CUserCharacInfo/SetOpencoin.inc"





#include "CUserCharacInfo/SetOneDayLetheFlag2ND.inc"



#include "CUserCharacInfo/SetOneDayLetheFlag.inc"



#include "CUserCharacInfo/SetLoginTick.inc"


#include "CUserCharacInfo/SetIsEndLetheAtLogin.inc"



#include "CUserCharacInfo/SetInvisible.inc"


#include "CUserCharacInfo/SetFirstTimeLogin.inc"


#include "CUserCharacInfo/SetCurRevengeDungeonCount.inc"



#include "CUserCharacInfo/SetCurExpEquipSlotStat.inc"



#include "CUserCharacInfo/SetCurCharacSuperState.inc"



#include "CUserCharacInfo/SetCurCharacSkillTreeIndex.inc"



#include "CUserCharacInfo/SetCurCharacRevengeData.inc"



#include "CUserCharacInfo/SetCurCharacOption.inc"





#include "CUserCharacInfo/SetCurCharacOnlinePreliminaryTeam.inc"



#include "CUserCharacInfo/SetCurCharacMaxEquipLevel.inc"



#include "CUserCharacInfo/enableSaveCharacManageInfo.inc"

#include "CUserCharacInfo/SetCurCharacLuckPointUpdateTimeCount.inc"



#include "CUserCharacInfo/SetCurCharacLeaveTimeout.inc"



#include "CUserCharacInfo/SetCurCharacGhost.inc"



#include "CUserCharacInfo/SetCurCharacExpertJobType.inc"



#include "CUserCharacInfo/enableSaveCharacInfo.inc"

#include "CUserCharacInfo/SetCurCharacExpertJobExp.inc"



#include "CUserCharacInfo/SetCurCharacExpertJob.inc"



#include "CUserCharacInfo/SetCurCharacEmotion.inc"


#include "CUserCharacInfo/SetCurCharacChaosPoint.inc"



#include "CUserCharacInfo/SetCurCharacChaosKillTime.inc"



#include "CUserCharacInfo/SetCurCharacAssaultedUserId.inc"



#include "CUserCharacInfo/SetCurCharacAssaultMoney.inc"



#include "CUserCharacInfo/SetCurCharacAssaultFatigueCount.inc"



#include "CUserCharacInfo/SetCharacStartPlayTick.inc"

#include "CUserCharacInfo/SetAssaultPlace.inc"



#include "CUserCharacInfo/ResetRevengeDungeonClear.inc"



#include "CUserCharacInfo/ResetPowerWarAssaultRecord.inc"



#include "CUserCharacInfo/ResetCurCharacEventItemCount.inc"



#include "CUserCharacInfo/ResetCurCharacEventAddItemCount.inc"



#include "CUserCharacInfo/ResetCurCharacDungeonPlayCount.inc"


#include "CUserCharacInfo/LiveCurCharac.inc"



#include "CUserCharacInfo/IsVillageAttackDBUpdate.inc"


#include "CUserCharacInfo/IsSaveDemensionInoutValue.inc"


#include "CUserCharacInfo/IsSaveCharacStat.inc"

#include "CUserCharacInfo/IsSaveCharacManageInfo.inc"

#include "CUserCharacInfo/IsSaveBloodInoutValue.inc"


#include "CUserCharacInfo/IsProperDungeonUser.inc"


#include "CUserCharacInfo/IsFirstTimeLogin.inc"


#include "CUserCharacInfo/IsExpItemAffected.inc"



#include "CUserCharacInfo/IsEndLetheAtLogin.inc"



#include "CUserCharacInfo/IsCurCharacVisible.inc"



#include "CUserCharacInfo/IsCurCharacLeaveTimeout.inc"


#include "CUserCharacInfo/IsBusyDisjointStore.inc"



#include "CUserCharacInfo/IsAvailableCurCharacTradeGoldDaily.inc"


#include "CUserCharacInfo/getCurCharacName.inc"



#include "CUserCharacInfo/InitFinishPoint.inc"



#include "CUserCharacInfo/IncrementCurCharacTradeGoldDaily.inc"


#include "CUserCharacInfo/IncreasePowerWarPoint.inc"



#include "CUserCharacInfo/IncProperDungeonClearCount.inc"



#include "CUserCharacInfo/IncPowerWarAssaultVictoryCount.inc"



#include "CUserCharacInfo/IncPowerWarAssaultCount.inc"



#include "CUserCharacInfo/IncFatigueBatteryCharging.inc"





#include "CUserCharacInfo/IncCurConditionEventStep.inc"





#include "CUserCharacInfo/IncCurConditionEventRewardStep.inc"





#include "CUserCharacInfo/IncCurCharacLuckPointUpdateTimeCount.inc"





#include "CUserCharacInfo/IncCurCharacLuckPoint.inc"


#include "CUserCharacInfo/IncCurCharacEventAddItemCount.inc"



#include "CUserCharacInfo/IncCurCharacDungeonPlayCount.inc"


#include "CUserCharacInfo/SetCurCharacDungeonPlayCount.inc"



#include "CUserCharacInfo/IncCurCharacChaosModeCount.inc"


#include "CUserCharacInfo/SetCurCharacChaosModeCount.inc"



#include "CUserCharacInfo/IncCurCharacChaosKillCount.inc"


#include "CUserCharacInfo/SetCurCharacChaosKillCount.inc"



#include "CUserCharacInfo/IncCurCharacChaosDieCount.inc"


#include "CUserCharacInfo/SetCurCharacChaosDieCount.inc"



#include "CUserCharacInfo/IncCurCharacAssaultFatigueCount.inc"



#include "CUserCharacInfo/IncCurCharacAssaultCount.inc"


#include "CUserCharacInfo/SetAssaultCount.inc"



#include "CUserCharacInfo/GmSetCurCharacChaosPoint.inc"



#include "CUserCharacInfo/GetWinPoint.inc"

#include "CUserCharacInfo/GetWarAreaKillCountTotal.inc"



#include "CUserCharacInfo/GetWarAreaKillCount.inc"


#include "CUserCharacInfo/GetUserStateByAssault.inc"



#include "CUserCharacInfo/GetUserPowerWarPoint.inc"


#include "CUserCharacInfo/GetReliablePerson.inc"

#include "CUserCharacInfo/GetProperDungeonClearCount.inc"


#include "CUserCharacInfo/GetPowerWarProcessInfoLastUpdateTime.inc"


#include "CUserCharacInfo/GetPowerWarLastPlayTime.inc"

#include "CUserCharacInfo/SetIsInitSkillFlag.inc"


#include "CUserCharacInfo/SetIsInitSkillFlag2ND.inc"


#include "CUserCharacInfo/IncVillageAttackCount.inc"


#include "CUserCharacInfo/GetPowerWarAssaultVictoryCount.inc"


#include "CUserCharacInfo/GetPowerWarAssaultCount.inc"


#include "CUserCharacInfo/GetOverTradeGoldCount.inc"

#include "CUserCharacInfo/GetOpencoin.inc"


#include "CUserCharacInfo/GetOneDayLetheFlag2ND.inc"


#include "CUserCharacInfo/GetOneDayLetheFlag.inc"


#include "CUserCharacInfo/GetLoginTick.inc"



#include "CUserCharacInfo/GetFinishPointTotal.inc"




#include "CUserCharacInfo/GetFinishPoint.inc"



#include "CUserCharacInfo/GetFatigueBatteryCharging.inc"


#include "CUserCharacInfo/GetFatigue.inc"


#include "CUserCharacInfo/GetExpAffectRate.inc"



#include "CUserCharacInfo/GetCurVillageAttackCount.inc"


#include "CUserCharacInfo/GetCurRevengeDungeonCount.inc"


#include "CUserCharacInfo/GetCurExEquipSlotStat.inc"


#include "CUserCharacInfo/GetCurConditionEventStep.inc"


#include "CUserCharacInfo/GetCurConditionEventRewardStep.inc"


#include "CUserCharacInfo/GetCurCharacSuperState.inc"


#include "CUserCharacInfo/GetCurCharacSkillTreeIndex.inc"


#include "CUserCharacInfo/GetCurCharacOnlinePreliminaryTeamName.inc"




#include "CUserCharacInfo/GetCurCharacOnlinePreliminaryTeamId.inc"




online_preliminary::COnlinePreliminaryTeam*
#include "CUserCharacInfo/GetCurCharacOnlinePreliminaryTeam.inc"



#include "CUserCharacInfo/GetCurCharacMaxEquipLevel.inc"


#include "CUserCharacInfo/GetCurCharacLuckPointUpdateTimeCount.inc"


#include "CUserCharacInfo/GetCurCharacExpertJobType.inc"


#include "CUserCharacInfo/GetCurCharacExpertJobExp.inc"


#include "CUserCharacInfo/GetCurCharacExpertJob.inc"


#include "CUserCharacInfo/GetCurCharacEventItemCount.inc"


#include "CUserCharacInfo/GetCurCharacEventAddItemCount.inc"


#include "CUserCharacInfo/GetCurCharacEmotion.inc"


#include "CUserCharacInfo/GetCurCharacDungeonPlayCount.inc"


#include "CUserCharacInfo/GetCurCharacChaosStateTime.inc"


#include "CUserCharacInfo/GetCurCharacChaosResponTime.inc"


#include "CUserCharacInfo/GetCurCharacChaosModeCount.inc"


#include "CUserCharacInfo/GetCurCharacChaosKillTime.inc"


#include "CUserCharacInfo/GetCurCharacChaosKillCount.inc"


#include "CUserCharacInfo/GetCurCharacChaosExp.inc"


#include "CUserCharacInfo/GetCurCharacChaosDieTime.inc"


#include "CUserCharacInfo/GetCurCharacChaosDieCount.inc"


#include "CUserCharacInfo/GetCurCharacBackupSkill2ND.inc"


#include "CUserCharacInfo/GetCurCharacBackupSkill.inc"


#include "CUserCharacInfo/GetCurCharacAssaultedUserId.inc"


#include "CUserCharacInfo/GetCurCharacAssaultMoney.inc"


#include "CUserCharacInfo/GetCurCharacAssaultFatigueCount.inc"


#include "CUserCharacInfo/GetContinuousItemInfo.inc"


#include "CUserCharacInfo/GetCurCharacEffectItemListR.inc"


#include "CUserCharacInfo/GetCurCharacCoolTimeItemListR.inc"

#include "CUserCharacInfo/GetCharacPlayTick.inc"


#include "CUserCharacInfo/GetCharacStartPlayTick.inc"

#include "CUserCharacInfo/GetAssaultPlace.inc"


#include "CUserCharacInfo/GetAssaultCount.inc"


#include "CUserCharacInfo/GainCurCharacChaosExp.inc"


#include "CUserCharacInfo/FindItemLock.inc"


#include "CUserCharacInfo/EnablePeriodExpItem.inc"




#include "CUserCharacInfo/SetExpAffectItemEffect.inc"



#include "CUserCharacInfo/EnablePeriodExpDouble.inc"




#include "CUserCharacInfo/SetIsAffectedExpDouble.inc"





#include "CUserCharacInfo/DieCurCharac.inc"



#include "CUserCharacInfo/get_charac_level.inc"


#include "CUserCharacInfo/getCurCharacVill.inc"



#include "CUserCharacInfo/SetCurCharacChaosResponTime.inc"



#include "CUserCharacInfo/SetCurCharacChaosDieTime.inc"



#include "CUserCharacInfo/IsCurCharacGhost.inc"


#include "CUserCharacInfo/GetCurCharacChaosPoint.inc"


#include "CUserCharacInfo/DecreaseWarPoint.inc"


#include "CUserCharacInfo/DecreaseRevengeDungeonTicket.inc"



#include "CUserCharacInfo/DecreasePowerWarPoint.inc"



#include "CUserCharacInfo/DecFatigueBatteryCharging.inc"





#include "CUserCharacInfo/DecCurCharacLuckPoint.inc"


#include "CUserCharacInfo/SetCurCharacLuckPoint.inc"



#include "CUserCharacInfo/GetCurCharacLuckPoint.inc"


#include "CUserCharacInfo/ClearUselessEffectItem.inc"


#include "CUserCharacInfo/GetCurCharacEffectItemList.inc"


#include "CUserCharacInfo/ClearUselessCooltimeItem.inc"


#include "CUserCharacInfo/GetCurCharacCoolTimeItemList.inc"


#include "CUserCharacInfo/ClearProperDungeonUser.inc"



#include "CUserCharacInfo/ClearProperDungeonClearCount.inc"


#include "CUserCharacInfo/SetProperDungeonClearCount.inc"



#include "CUserCharacInfo/ClearCurConditionEventStep.inc"


#include "CUserCharacInfo/SetCurConditionEventStep.inc"



#include "CUserCharacInfo/ClearCurConditionEventRewardStep.inc"


#include "CUserCharacInfo/SetCurConditionEventRewardStep.inc"



#include "CUserCharacInfo/ChangeFinishPoint.inc"


#include "CUserCharacInfo/CUserCharacInfo.inc"


#include "CUserCharacInfo/resetSaveFlags.inc"


#include "CUserCharacInfo/AddCurCharacChaosPoint.inc"



#include "CUserCharacInfo/enableSaveCharacStat.inc"

#include "CUserCharacInfo/GetTenThousandPercentage.inc"

