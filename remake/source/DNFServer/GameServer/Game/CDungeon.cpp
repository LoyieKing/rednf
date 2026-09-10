// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDungeon/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// df_game_r 还原 —— CDungeon / CDungeon_TowerOfDespair / STDungeonScript
//（G2 副本链）。
// 逐函数对照 docs/class_func_reports/CDungeon.md、CDungeon_TowerOfDespair.md
// 与 ORIG df_game_r 反汇编实现；目标 identical 口径。
// 支撑类型（MazeScript/stDungeonAssignItem_t/STWarRoomData/...）的 ORIG W 符号
// 也在本 TU 内实现。
// [b28d-solo-CDungeon2 收口] 清单 5 件：_do_after_dungeon_start NEAR→AE；
// GetRandMazebyPartyCount DIFF→NEAR（帧 0x40 / divisor 槽 -0x2c 与 ORIG 全等）；
// set_dungeon 真实残差 537→20 行（帧 0x2bc / nameBuf 槽 -0x29c 与 ORIG 全等，
// 余 3×movzbl 异型中转 N18a + 1 岛 N112 + 中间区 4 槽 N82）；
// ClearRewardData::operator= / IncreaseEnterCount 复验仍 N18a 封顶。
// TU 口径：identical+ae 1767→1768、near 1（换件）、diff 6→5；p167 五件全 CLEAR。
#include "CDungeon.h"

#include <string.h>
#include <stdlib.h>

#include <algorithm>

#include "CMap.h"
#include "GameTypes.h"
#include "LogManager.h"
#include "DNFFunctionLib.h"
#include "CInventory.h"             // check_item_exist/GetInvenSlot/delete_item
// [Lane37CUser 2026-09-10 解环] CInventory.h 的 `#include "CUser.h"` 已改前向声明
//   （解 CUser.h ⇄ CInventory.h 环）⇒ 依赖传递获得 CUser 完整定义的 TU 需自行直补。
#include "CUser.h"
#include "PacketGuard.h"            // 门票回执包
#include "GlobalData.h"             // GlobalData::s_systemTime_/s_statistic_proxy_mgr
#include "CServerProxyMgr.h"        // GetServerProxy 完整模板
#include "CStatisticServerProxy.h"  // 统计上报
#include "DataManagerInitDepends.h" // TOD_UserState/TOD_Layer（绝望之塔扩展数据）
#include "CBoosterGage.h"           // charac_expand::CData::alter（脏标记）

// ============================================================================
// 支撑类型：GridScript / RidableObject / RidableScript / DungeonClearCondition
// ============================================================================

// ============================================================================
// CDungeon 弱访问器 / 自包含逻辑（ORIG 地址见各函数注释；
// ctor/dtor/set_dungeon 及 GameStubs.cpp 已有桩的方法不在此重复定义）
// ============================================================================

#include "CDungeon/getDungeonKind.inc"
}

#include "CDungeon/isNoFatigueDungeon.inc"
}

#include "CDungeon/get_blood_dungeon_type.inc"
}

#include "CDungeon/getQuestNpcDungeon.inc"
}

#include "CDungeon/get_limit_party_count.inc"
}

#include "CDungeon/get_limit_Inout_count.inc"
}

#include "CDungeon/limitOfStackableItemInTower.inc"
}

#include "CDungeon/getHeroesRequiredQuest.inc"
}

#include "CDungeon/getAICharacterAppearRate.inc"
}

#include "CDungeon/getRandomTowerMaxStage.inc"
}

#include "CDungeon/getDeathTowerMaxStage.inc"
}

#include "CDungeon/getDeathTowerMapIndex.inc"
}

#include "CDungeon/getTowerMaxClearRewardItemNum.inc"
}

#include "CDungeon/getEventMonsterTotalProb.inc"
}

#include "CDungeon/getTryGenEventMonsterCnt.inc"
}

#include "CDungeon/getTryGenEventMonsterProb.inc"
}

#include "CDungeon/getSecondEventMonsterTotalProb.inc"
}

#include "CDungeon/getTryGenSecondEventMonsterCnt.inc"
}

#include "CDungeon/getTryGenSecondEventMonsterProb.inc"
}

// [b28d-solo-CBossStage R2] 随 CDungeon.h 声明同步（uchar→bool，调用点 P292 判据）。
#include "CDungeon/hasTowerItemDrop.inc"
}

#include "CDungeon/GetDummyAppearCount.inc"
}

#include "CDungeon/IsHaveDummyMap.inc"
}

#include "CDungeon/isRiskDungeon.inc"
}

#include "CDungeon/IsOpenEvenEnemy.inc"
}

#include "CDungeon/GetPartyMemberCoinLimit.inc"
}

#include "CDungeon/get_exp_weight.inc"
}
#include "CDungeon/getDropItems.inc"
}
#include "CDungeon/getObjectDropItems.inc"
}

#include "CDungeon/getSpecialPassiveObjectItems.inc"
}

#include "CDungeon/getEventMonsters.inc"
}

#include "CDungeon/getSecondEventMonsters.inc"
}

#include "CDungeon/getRandomTowerMapIndexes.inc"
}

#include "CDungeon/get_dimension_min_partymem.inc"
}

#include "CDungeon/GetQuestMazeScriptVector.inc"
}

// ---- 迁移自 GameStubs.cpp 的 CDungeon 桩方法（ORIG 弱符号，签名已核对）----
#include "CDungeon/CDungeon.inc"
}
#include "CDungeon/dtor_CDungeon.inc"
}

#include "CDungeon/get_index.inc"
}

#include "CDungeon/get_min_level.inc"
}

#include "CDungeon/get_standard_level.inc"
}

#include "CDungeon/GetDungeonName.inc"
}
#include "CDungeon/get_dimension_possible.inc"
}

#include "CDungeon/isTowerOfDespairDungeon.inc"
}

#include "CDungeon/GetType_DungeonInHeritance.inc"
}

#include "CDungeon/isTournamentDungeon.inc"
}

#include "CDungeon/getTournamentRoundFatigue.inc"
}

#include "CDungeon/IsEnterEachMap.inc"
}

// ORIG _ZN12CDataManager18set_dimensionInoutEic / _ZN12CDataManager21set_limit_inout_countEc15ENUM_BLOOD_TYPE
// 真实 C++ 成员实现（权威声明在 CDataManager.h），语义按 ORIG 反汇编：
//   set_dimensionInout（080eed82）：idx>5 或 idx<0 早退，再写数组
//   set_limit_inout_count（08374c82）：写 m_limitInoutCount[type]
#include "CDungeon/set_dimensionInout.inc"
}

#include "CDungeon/set_limit_inout_count.inc"
}

#include "CDungeon/set_dungeon.inc"
}

// ---- 绝望之塔（TOD）----
// ORIG：GetCharacExpandData(0xd) 返回 TOD_UserState（charac_expand 扩展数据，
// 权威布局见 DataManagerInitDepends.h；IncreaseEnterCount/getEnterCount/
// getTodayEnterLayer/TOD_Layer::GetLayer 均为 ORIG 真实符号）。

// ORIG T 0x0864388a（报告 TOD_UserState/IncreaseEnterCount.md）：enterCount(+0x10)
// 与 lastEnterLayer(+0x16) 各自 ++、lastEnterTime(+0x18)=getCurSec；当日层槽
// (+0x14) 按 ORIG 是内嵌 TOD_Layer——判 0 走真实 call TOD_Layer::GetLayer
// (W 0x0822ec40, this+0x14)，非字段直读。（Sec 批：TOD_UserState +0x8 段已内嵌
// SIG_REQUEST_TOD_USER_STATE，m_todayLayer 本体即 TOD_Layer，直调免 cast。）
// [b22判词·跨版本] 残余 1 insn：ORIG 4.1 对 ushort 回写生成 `add $0x1,%eax;
// mov %eax,%edx`，4.4 恒融合为 `lea 0x1(%eax),%edx`（三变体实测均 lea，见
// 范式库跨版本豁免表 movzwl 段）；`movl $0x0,(%esp)` 系 .o R_386_32 数据地址
// reloc 占位（§6.3），relink 后即 &GlobalData::s_systemTime_。语义等价。
// [b28c-lane17] fresh 复验确认：字段已 unsigned short（movzwl 宽度/偏移全对），
// 唯一残差即该 lea-vs-add+mov 折叠 ⇒ N18a（单段 regmove 不可造）封顶。
#include "CDungeon/IncreaseEnterCount.inc"
}

// 注：保持 POD（无 user ctor）——GCC 4.4 __attribute__((packed)) 对含非 POD
// 成员的外层 struct 不生效，会致 Key 偏移 4 对齐（0xc）而非 ORIG 0xa。
// [b28d-solo-CDungeon2] 成员名保持与 TowerOfDespairStatistic_Key.h 逐字一致（ODR），
// 但语义更正：m_0@+0 = packet+0xa（_do_after_dungeon_start/ClearDungeon/Statics
// 全无写读点，占位）、m_4@+4 = packet+0xe（当日层/层数写入口；ORIG 0x0834d8ec
// `mov %eax,-0x31(%ebp)`，gts 基址 -0x3f ⇒ +0xe）。权威头
// shared/packet/include/Packet_TowerOfDespair_Statistic_GTS.h 同布局（m_padA/m_layer）。
struct TowerOfDespairStatistic_Key
{
    unsigned int m_0;   // +0x00（packet+0xa，无写/读点，占位）
    unsigned int m_4;   // +0x04（packet+0xe，当日层/层数写入口）

    TowerOfDespairStatistic_Key();
};

#include "CDungeon/TowerOfDespairStatistic_Key.inc"
}

// Packet_TowerOfDespair_Statistic_GTS（ORIG ctor 0x08374cba W，总尺寸 0x17）：
// PacketHeader(0x271c,0x17)[10] + Key[8] + byte flag + uint accId。
// 注：用 #pragma pack（而非 __attribute__((packed))）——GCC 4.4 的 attribute
// packed 对含非 POD 成员（Key 有 user ctor）的外层 struct 不生效，会致 Key
// 偏移 4 对齐（0xc）而非 ORIG 0xa。
#pragma pack(push, 1)
struct Packet_TowerOfDespair_Statistic_GTS
{
    PacketHeader m_header;   // +0x00（packed 0xa 字节）
    TowerOfDespairStatistic_Key m_key;  // +0x0a
    unsigned char m_flag;    // +0x12
    unsigned int m_accId;    // +0x13

    Packet_TowerOfDespair_Statistic_GTS();
};
#pragma pack(pop)

#include "CDungeon/Packet_TowerOfDespair_Statistic_GTS.inc"
}

#include "CDungeon/CDungeon_TowerOfDespair.inc"
}
#include "CDungeon/_do_after_dungeon_start.inc"
}
#include "CDungeon/_do_after_dungeon_start_1.inc"
}

#include "CDungeon/GetMaze.inc"
}

#include "CDungeon/GetRandMaze.inc"
}

#include "CDungeon/GetRandMazebyPartyCount.inc"
}

#include "CDungeon/GetStartMap.inc"
}

#include "CDungeon/get_clear_item.inc"
}

#include "CDungeon/get_clear_item_1.inc"
}

#include "CDungeon/getLimitCoinDiff.inc"
}

#include "CDungeon/checkFreeRevivalCondition.inc"
}

#include "CDungeon/get_dimension_member_count.inc"
}

#include "CDungeon/clear.inc"
}

#include "CDungeon/GridScript.inc"
}

#include "CDungeon/RidableObject.inc"
}


#include "CDungeon/clear_1.inc"
}

#include "CDungeon/RidableScript.inc"
}

#include "CDungeon/dtor_RidableScript.inc"
}

#include "CDungeon/DungeonClearCondition.inc"
}

#include "CDungeon/clear_2.inc"
}

// ============================================================================
// MazeScript（0x74 字节）
// ============================================================================
#include "CDungeon/clear_3.inc"
}

#include "CDungeon/MazeScript.inc"
}

#include "CDungeon/dtor_MazeScript.inc"
}

#include "CDungeon/getGrid.inc"
}

// ============================================================================
// stDungeonAssignItem_t / RandomList / 事件怪结构
// ============================================================================

#include "CDungeon/clear_4.inc"
}

#include "CDungeon/stDungeonAssignItem_t.inc"
}

#include "CDungeon/dtor_stDungeonAssignItem_t.inc"
}

// ============================================================================
// STMonsterTypeSpawnProb / STDeadTowerData / STWarRoomData / RequiredItem
// ============================================================================

// ORIG W 0x088f2c64（报告缺失兜底：nm 定址+objdump 切片；清单给
// docs/class_func_reports/CCharacter/clear.md 系撞名报告，签名为
// _ZN10CCharacter5clearEv，已按 §6.3 弃用）：clear 整体 memset(this,0,0x10)。
// [b22判词·跨版本] 残余 1 序差：4.4 填 memset 出参槽先于取 this，4.1 先生成
// `mov 0x8(%ebp),%eax`（指令集相同仅两 mov 对调，三变体实测 4.4 无法复现前序）；
// call 目标 reloc 即 memset（§6.3 占位假阳性）。语义等价。
#include "CDungeon/clear_5.inc"
}

#include "CDungeon/STMonsterTypeSpawnProb.inc"
}

#include "CDungeon/clear_6.inc"
}

#include "CDungeon/STDeadTowerData.inc"
}

#include "CDungeon/dtor_STDeadTowerData.inc"
}

#include "CDungeon/STWarRoomData.inc"
}

#include "CDungeon/clear_7.inc"
}

#include "CDungeon/copy.inc"
}

#include "CDungeon/dtor_STWarRoomData.inc"
}

#include "CDungeon/RequiredItem.inc"
}

// clear 定义移到 ctor 之后（NL7 实证：callee 后置时 4.4 unit-at-a-time 不塌缩
// ctor outgoing，保持 ORIG sub $0x18）。
#include "CDungeon/clear_8.inc"
}
#include "CDungeon/clear_9.inc"
}

#include "CDungeon/ClearRewardData.inc"
}

#include "CDungeon/dtor_ClearRewardData.inc"
}

advancealtar::ClearRewardData& advancealtar::ClearRewardData::operator=(
    const ClearRewardData& other)
{
    // [b28d-solo-CDungeon 判词 · N18a 封顶] ORIG 0x08376c9c / our 27（ORIG 28，
    // gap 1）；p167 1=1 CLEAR。语义已按 ORIG 修正（--n 前置测试只拷前 2 个元素，
    // P171 照抄）；指针步进 do-while（esi=dst/ebx=src 各 +0xc、edi=2 递减到 -1、
    // `setne/test/jne`）；声明序 dst/n/src 对齐 ORIG 装载序（esi→edi→ebx）。
    // 残余唯一：ORIG `mov 0x8(%ebp),%eax; mov %eax,%esi` 被 4.4 折成单条
    // `mov 0x8(%ebp),%esi`（单段 regmove 不可造）。本轮再试 `&m_vec[0]` 形仍折
    // 单段；g1-g6/h1-h6 十二形探针（语句表达式/整型-void*-char* 中转/具名引用）+
    // register 存储类保留均不可翻 ⇒ N18a。[b28d-solo-CDungeon2 复验] fresh 编译
    // 仍唯一残差；根因归 N102（结构首成员数组 m_vec@+0 退化取址，4.4 恒折单条
    // disp），不再试形。
    register std::vector<advancealtar::_RewardItem>* dst = m_vec;
    register int n = 2;
    register const std::vector<advancealtar::_RewardItem>* src = other.m_vec;
    // ORIG 0x08810d8a 为 `--n != -1` 前置测试 while：n=2 只拷贝 2 个元素（P171 照抄）。
    while (--n != -1)
    {
        *dst = *src;
        ++dst;
        ++src;
    }
    return *this;
}

#include "CDungeon/SurvivalRewardData.inc"
}

#include "CDungeon/dtor_SurvivalRewardData.inc"
}

advancealtar::SurvivalRewardData&
advancealtar::SurvivalRewardData::operator=(const SurvivalRewardData& other)
{
    m_vec = other.m_vec;
    return *this;
}

#include "CDungeon/Clear.inc"
}

#include "CDungeon/TournamentRewardItemRate.inc"
}

#include "CDungeon/dtor_TournamentRewardItemRate.inc"
}

TournamentRewardItemRate& TournamentRewardItemRate::operator=(
    const TournamentRewardItemRate& other)
{
    m_items = other.m_items;
    m_c = other.m_c;
    return *this;
}

#include "CDungeon/stConditionEnterBossRoom_t.inc"
}

#include "CDungeon/dtor_stConditionEnterBossRoom_t.inc"
}

#include "CDungeon/clear_10.inc"
}

#include "CDungeon/RecoveryStairInfo.inc"
}

#include "CDungeon/dtor_RecoveryStairInfo.inc"
}

#include "CDungeon/clear_11.inc"
}

#include "CDungeon/STTowerDialog.inc"
}

#include "CDungeon/dtor_STTowerDialog.inc"
}

STTowerDialog& STTowerDialog::operator=(const STTowerDialog& other)
{
    m_index = other.m_index;
    m_name = other.m_name;
    m_flag8 = other.m_flag8;
    m_flag9 = other.m_flag9;
    m_flagA = other.m_flagA;
    return *this;
}

#include "CDungeon/clear_12.inc"
}

#include "CDungeon/STTowerFullDialog.inc"
}

#include "CDungeon/dtor_STTowerFullDialog.inc"
}

// ---- ORIG W 弱符号发射段（头内联定义，本 TU 无 odr-use 故未发射）----
//   TournamentRewardItem  C1/C2 @0x088f2dcc（三字段清零）、D1/D2 @0x088f9ef6（空体）
//   STNamedMonsterMapPos  C1/C2 @0x088f2d6c（m_y=0; m_x=m_y）、D1/D2 @0x088f8cbc（空体）
// 真实构造/析构强制发射，不引入导出符号。
namespace
{
#include "CDungeon/emitDungeonRewardWeakSymbols.inc"
}
}

