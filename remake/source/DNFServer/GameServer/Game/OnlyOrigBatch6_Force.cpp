// ============================================================================
// OnlyOrigBatch6_Force.cpp —— 本批仅 ORIG 符号的落地（构造/析构/copy 族）。
//
// 全部目标符号为 ORIG 头内联弱符号（nm W），重建头内联定义已补全（SigTypes
// C/D/E/B、SIG_SAVE_EXPAND_EQUIPSLOT、STGuildSkill、BreakAwayPreventSystem、
// CDungeon、CBattle_Field_shared、CUser、EquipmentParameterInfo、CMap、
// STComputeDamageVariable、STStackableScript、SpecialItemHandler、
// W8LT_PassiveObjectScript、TODLayer、CTournamentDungeonReward、CStackableItem、
// QuestClear）；本 TU 通过显式构造/拷贝/析构（P147/P149 force 发射）使编译器
// 按 ODR 使用点发射同名 W 符号。STGrowthPowerData 两 getter 与 ScanAll ×2 为
// T 强符号，已落地于 CDataManagerScripts.cpp / DNFLexWrapper.cpp。
// ============================================================================

#include "SigTypes.h"                  // SIG_* 全部（C/D/E/B 分片）
#include "STGuildSkill.h"
#include "BreakAwayPreventSystem.h"    // STRewardData
#include "CDungeon.h"                  // STTowerDialog/TournamentRewardRate/UseCoinDungeonDiff
#include "CBattle_Field_shared.h"      // STKillMonsterData
#include "CUser.h"                     // STPremiumItemData
#include "EquipmentParameterInfo.h"    // STResultItemCnt
#include "CMap.h"                      // STAssignBloodSystem 等
#include "STComputeDamageVariable.h"
#include "STStackableScript.h"         // STStackablePackageData/STEnchantSystemMapData/Key
#include "SpecialItemHandler.h"
#include "W8LT_PassiveObjectScript.h"  // STPassiveObjectScript/SpeedCtrlPassiveObjInfo
#include "TODLayer.h"
#include "CTournamentDungeonReward.h"  // TournamentRewardType
#include "CStackableItem.h"            // StatChangeInfo

namespace
{

// P147 force 发射：每个 ODR 使用点生成对应 W 符号（C1/C2 同址别名、D1/D2 同址）。
void emitBatch6WeakSymbols()
{
    // ---- SIG_* 构造（C1/C2）----
    SIG_EVENT_STAGE_LOG s1;
    SIG_ITEM_DICTIONARY_FLAG s2;
    SIG_ITEM_MAKING_SKILL s3;
    SIG_LOAD_INVENTORY s4;
    SIG_LOAD_KILL_MONSTER_INFO s5;
    SIG_LOAD_QUEST s6;
    SIG_LOAD_SKILL s7;
    SIG_POSTAL_CREATURE_TEMP_PERIOD s8;
    SIG_QUEST_SHOP_DATA s9;
    SIG_REDEEMINFO_LIST_TO_DB s10;
    SIG_REQUEST_PVP_MISSION_LIST s11;
    SIG_REQUEST_TOD_USER_STATE s12;
    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO s13;
    SIG_SAVE_DIMENSION_RENT_INFO s14;
    SIG_SAVE_DIMENSION_RENT_ITEM s15;
    SIG_SAVE_EXPAND_EQUIPSLOT s16;
    SIG_SAVE_PC_ROOM_PLAY_TIME s17;
    SIG_SAVE_PC_ROOM_RENT_INFO s18;
    SIG_SAVE_PC_ROOM_RENT_ITEM s19;
    TAG_CHARAC_ETC_DATA s20;
    TAG_CHARAC_INVEN_DATA s21;

    // ---- SIG_LOAD_ETC 析构（D1/D2，构造同址 C1 一并发射）----
    SIG_LOAD_ETC s22;

    // ---- ST 家族构造 ----
    STGuildSkill s23;
    STRewardData s24;
    STTowerDialog s25;
    STTowerDialog s25b(s25);        // C1ERKS/C2ERKS
    STKillMonsterData s26;
    STPremiumItemData s27;
    STResultItemCnt s28;
    STAssignBloodSystem s29;
    STAssignBloodPhaseTime s30;
    STAssignDefenseSystem s31;
    STAssignDefensePhaseTime s32;
    STComputeDamageVariable s33;
    STEnchantSystemMapData s34;
    STEnchantSystemMapKey s35;
    STStackablePackageData s36;
    CStackableItem::StatChangeInfo s37;
    TournamentRewardRate s38;
    TournamentRewardType s39;
    UseCoinDungeonDiff s40;
    TOD_Layer s41(0);
    TOD_Layer s41b(s41);            // C1ERKS/C2ERKS

    // ---- 析构类（D1/D2）----
    STPassiveObjectScript s42;
    SpeedCtrlPassiveObjInfo* sp2 = new SpeedCtrlPassiveObjInfo();
    sp2->~SpeedCtrlPassiveObjInfo();   // 显式析构 → 发射隐式 D1/D2 W（ORIG 0x08a4eab2）
    delete sp2;
    SpecialItemHandler* sp = new SpecialItemHandler(0);
    delete sp;

    (void)s1; (void)s2; (void)s3; (void)s4; (void)s5; (void)s6; (void)s7;
    (void)s8; (void)s9; (void)s10; (void)s11; (void)s12; (void)s13; (void)s14;
    (void)s15; (void)s16; (void)s17; (void)s18; (void)s19; (void)s20; (void)s21;
    (void)s22; (void)s23; (void)s24; (void)s25; (void)s25b; (void)s26;
    (void)s27; (void)s28; (void)s29; (void)s30; (void)s31; (void)s32; (void)s33;
    (void)s34; (void)s35; (void)s36; (void)s37; (void)s38; (void)s39; (void)s40;
    (void)s41; (void)s41b; (void)s42; (void)sp2;
}

}  // namespace
