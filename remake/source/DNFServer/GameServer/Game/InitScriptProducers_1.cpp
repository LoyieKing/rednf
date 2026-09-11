// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InitScriptProducers_1/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CDataManager.h"
#include "sync_script.h"   // STEquipmentPartSetPiece 完整类型（DataManagerInitDepends.h 前置依赖）
#include "DataManagerInitDepends.h"
#include "DNFLexWrapper.h"
#include "LogManager.h"

#include <cstdio>
#include <cstring>

// ---- 外部定义 ----
int  getCharacterJobToEnumValue(std::string& job);                     // CDataManager.cpp 0x89bbc0f
int  LoadRewardDataByGrowType(stLevelUpRewardItemScript& script,
                              std::string endTag);                     // InitScriptProducers_2.cpp

// ---- 布局自证（与 ORIG sizeof 一致性锚点）----
namespace {

struct LayoutAsserts
{
    // LevelUpRewardItemTableKey：ctor/clear 0x0891bac0/0x0891bad4 → {ushort,uchar,uchar}
    char check_key[sizeof(LevelUpRewardItemTableKey) == 4 ? 1 : -1];
    // LevelUpRewardItemData：clear 0x081b88d0 写两 int（-1/0）
    char check_data[sizeof(LevelUpRewardItemData) == 8 ? 1 : -1];
    // StackableItemData：LoadRewardDataByNoType 栈上两连续 int
    char check_stackable[sizeof(StackableItemData) == 8 ? 1 : -1];
    // stGrowthCapsuleScript：C1 0x0837114a 五个 int
    char check_capsule[sizeof(stGrowthCapsuleScript) == 0x14 ? 1 : -1];
    // stSeriaBlessingScript::Value：C1 0x08693aa0 五个 int
    char check_seria_value[sizeof(stSeriaBlessingScript::Value) == 0x14 ? 1 : -1];
    // WorldDropInfo：槽位步长 0x14（m_worldDrop1[201]+0x6734 → m_worldDrop2@+0x76e8）
    char check_drop[sizeof(WorldDropInfo) == 0x14 ? 1 : -1];
    // channel_info_t / channel_dungeon_info_t：ctor 0x088dd2a8/0x088dd34a
    char check_channel_info[sizeof(channel_info_t) == 0x40 ? 1 : -1];
    char check_channel_dungeon[sizeof(channel_dungeon_info_t) == 0x10 ? 1 : -1];
};

}  // namespace

#include "InitScriptProducers_1/importChannelScript.inc"

// ============================================================================
// AccountCargoScript（Etc/AccountCargo.etc 域）
// ============================================================================

#include "InitScriptProducers_1/clear.inc"

#include "InitScriptProducers_1/importAccountCargoScript.inc"

// ============================================================================
// Channel（Etc/channel_info.etc 域）
// ============================================================================

#include "InitScriptProducers_1/channel_info_t.inc"

#include "InitScriptProducers_1/dtor_channel_info_t.inc"

// b28d-solo-InitScriptProducers_1: AE（修：m_gridPercent 加 union 块视图
// `m_gridBlock = o.m_gridBlock`，复现 ORIG 0x088dd457 src 缓存 %edx + %ecx 中转的
// 0x28 字节结构块拷贝（P246）；逐元素循环生 scaled 索引 + 循环变量槽致帧 0x28）。
// ORIG W 0x088dd3c6：逐成员拷贝（string×3 赋值 + type + pBonusRates + bonusRate +
// floats 按 dword 循环拷贝），与编译器逐字段序一致。
channel_info_t& channel_info_t::operator=(const channel_info_t& o)
{
    m_name = o.m_name;
    m_channelType = o.m_channelType;
    m_dungeonName = o.m_dungeonName;
    m_dungeonKey = o.m_dungeonKey;
    m_pBonusRates = o.m_pBonusRates;
    m_bonusRate = o.m_bonusRate;
    // ORIG 0x088dd457..0x088dd472：src 缓存 %edx + %ecx 中转的 0x28 字节整体块
    // 拷贝（P246）⇒ 结构视图整体赋值，非逐元素循环。
    m_gridBlock = o.m_gridBlock;
    return *this;
}

// channel_dungeon_info_t 的 C1/D1/op= 为 ORIG W 隐式合成（P299/CWG434：ORIG
// map::operator[] 0x088dd490 在构造临时对象前发 4×movl $0 前置零初始化）⇒
// 本 TU 不得写用户定义体（SweepInitProd6-2 已删 CDataManager.h 内声明）；
// 由 applyChannelScript 的 `channel_dungeon_info_t info;` 与 `map::operator[]`
// 使用点发射同名 W 符号（ORIG 0x45/0x45/0x35）。

#include "InitScriptProducers_1/applyChannelScript.inc"

// ---- Channel 类型名表（ORIG static _ZL19g_ChannelTypeString d 0x093f4e40，
//      20 项 ASCII 字符串指针，getChannelType/toChannelTypeString 共用）----
static const char* const g_ChannelTypeString[20] = {
    "CHANNEL_TEEN",                     // 0
    "CHANNEL_GENERAL",                  // 1
    "CHANNEL_BEGINNER",                 // 2
    "CHANNEL_TRADE",                    // 3
    "CHANNEL_PVP",                      // 4
    "CHANNEL_FREEPVP",                  // 5
    "CHANNEL_GUILD_WAR_PVP",            // 6
    "CHANNEL_TOURNAMENT_PVP",           // 7
    "CHANNEL_INTEGRATED_PVP",           // 8
    "CHANNEL_SCHOOL_LEAGUE_PVP",        // 9
    "CHANNEL_SCHOOL_TOURNAMENT_PVP",    // 10
    "CHANNEL_GORGE_OF_THE_DEAD",        // 11
    "CHANNEL_CASUAL_PVP",               // 12
    "CHANNEL_INTEGRATED_FREEPVP",       // 13
    "CHANNEL_ONLINE_PRELIMINARY_PVP",   // 14
    "CHANNEL_FAIR_PVP",                 // 15
    "CHANNEL_FAIR_FREEPVP",             // 16
    "CHANNEL_DEBUGGING",                // 17
    "CHANNEL_ATTACK_ZONE",              // 18
    "CHANNEL_MAX",                      // 19
};

// ORIG T 0x088dd190：越界返回 "UNKNOWN_CHANNEL_TYPE"，否则返回类型名字符串。
const char* toChannelTypeString(int type)
{
    if (type >= 0 && type <= 0x13)
        return g_ChannelTypeString[type];
    return "UNKNOWN_CHANNEL_TYPE";
}

#include "InitScriptProducers_1/getChannelType.inc"

#include "InitScriptProducers_1/traversalChannelScript.inc"

// ============================================================================
// WorldDrop（Etc/WorldDrop.etc、Etc/WorldDropWarArea.etc）
// ============================================================================

// ============================================================================
// WorldDrop（Etc/WorldDrop.etc、Etc/WorldDropWarArea.etc）
// ============================================================================

#include "InitScriptProducers_1/ImportWorldDrop.inc"

// ============================================================================
// Seria Blessing（Event/SeriaBlessing.evt）
// ============================================================================

#include "InitScriptProducers_1/ImportSeriaBlessingScript.inc"

// ============================================================================
// LevelUpRewardItem（Event/LevelUpRewardItem.evt）
// ============================================================================


#include "InitScriptProducers_1/ImportLevelUpRewardItemScript.inc"

// ============================================================================
// GrowthCapsule（Event/GrowthCapsule.evt）
// ============================================================================

#include "InitScriptProducers_1/ImportGrowthCapsuleScript.inc"

// ============================================================================
// NewAccountLevelUpToJob（Event/NewAccountLevelUpToJob.evt）
// ============================================================================

#include "InitScriptProducers_1/clear_1.inc"

#include "InitScriptProducers_1/set_data.inc"
// ---- StackableItemData（ORIG C1 0x0891bccc / clear 0x0891bce0，W）----
// clear() 定义在 CtorProducers_1.cpp（NL4 unit-at-a-time 塌缩机制：callee 同 TU
// 可见时 4.4 将本 ctor outgoing 帧塌缩 sub $0x4，ORIG 0x86b4144 域族 sub $0x18）。

#include "InitScriptProducers_1/StackableItemData.inc"

#include "InitScriptProducers_1/LoadRewardDataByNoType.inc"

#include "InitScriptProducers_1/ImportNewAccountLeventUpToJobScript.inc"

// ============================================================================
// BroadCastItem（Etc/BroadCastItem.etc）
// ============================================================================

#include "InitScriptProducers_1/ImportBroadCastItemScript.inc"

// ============================================================================
// 共享字符串工具（LoadRewardDataByGrowType 依赖，Main 裁定本批承载）
// ============================================================================

#include "InitScriptProducers_1/deleteTagBracketString.inc"
