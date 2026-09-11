// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ARAD/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ ARAD 命名空间类实现（批3：从 GameStubs.cpp 迁移，批4：Arad 脚本生命周期）。
// 依据 docs/class_func_reports/ARAD__Arad_DataManager/* + 各子对象报告 + ORIG elf 反汇编。
#include "AradServerStateMessage.h"
#include "Arad_EventPeriodDataManager.h"
#include "Arad_StatisticsEventItemList.h"
#include "AvatarFixedHiddenOptionServer.h"  // #1 注册项（本 TU 新建）
#include "AvatarRechargeServer.h"            // #2 注册项
#include "EmblemCompoundServer.h"            // #3 注册项（FixAradEmblemConvert 新建 TU）
#include "AvatarConvertServer.h"             // #4 注册项（FixAradEmblemConvert 新建 TU）
#include "LevelupSupportEventManger.h"      // #6 注册项
#include "CreateDnfEventSciprt.h"           // #8 注册项
#include "ARAD_FN_DeleteGameScript.h"   // FN_DeleteGameScript 唯一类声明（Arad_DataManager.h 不含）
#include "Arad_Script.h"
#include "CUser.h"          // CerashopAddRestrict::Manager（Init 注册）
#include "PacketGuard.h"    // notifyOpenMessageDialog 组包
#include "LogManager.h"
#include "DNFLexWrapper.h"  // parsing：limited_map 段 Load(g_dnfLex.getInputStreamBuffer())
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ARAD
{

// ============================================================================
// ScriptInterface（虚基接口）
// （ORIG nm 无 ScriptInterface 析构符号：析构隐式平凡，不提供定义——
//  见 Arad_DataManager.h ScriptInterface 注释。）

#include "ARAD/LoadScript.inc"


#include "ARAD/DestroyScript.inc"


#include "ARAD/RegistScript.inc"


#include "ARAD/Init.inc"


#include "ARAD/Arad_InterfaceGameScript.inc"


#include "ARAD/dtor_Arad_InterfaceGameScript.inc"

#include "ARAD/scriptDataName.inc"


}  // namespace ARAD

// ============================================================================
// 6 个脚本子对象位于【全局命名空间】（ORIG mangled _ZN13ExceptionItem... 等，
// 无 ARAD:: 前缀）——见 Arad_DataManager.h 说明。
// ============================================================================
// Arad_ScriptLoader（open/parsing 见 ScriptExtras.cpp）
// ============================================================================

#include "ARAD/ExceptionItem.inc"
#include "ARAD/dtor_ExceptionItem.inc"

#include "ARAD/parsing.inc"


#include "ARAD/find_index.inc"


#include "ARAD/PurchaseLimitItem.inc"
#include "ARAD/dtor_PurchaseLimitItem.inc"

#include "ARAD/parsing_1.inc"


#include "ARAD/find.inc"


#include "ARAD/AradBonusItem.inc"
#include "ARAD/dtor_AradBonusItem.inc"

#include "ARAD/parsing_2.inc"


#include "ARAD/find_1.inc"


#include "ARAD/Arad_Mileage_Data.inc"
#include "ARAD/dtor_Arad_Mileage_Data.inc"

#include "ARAD/parsing_3.inc"


#include "ARAD/find_2.inc"


#include "ARAD/default_value.inc"


#include "ARAD/Arad_OnePlusCardEvent_ItemList.inc"


#include "ARAD/dtor_Arad_OnePlusCardEvent_ItemList.inc"


#include "ARAD/parsing_4.inc"


#include "ARAD/find_item.inc"


#include "ARAD/find_item_1.inc"


#include "ARAD/is_limited_map.inc"


#include "ARAD/Arad_VillageAttackRewardItem.inc"
#include "ARAD/dtor_Arad_VillageAttackRewardItem.inc"

#include "ARAD/parsing_5.inc"


#include "ARAD/find_reward_item.inc"


#include "ARAD/find_ranking_reward_item.inc"
{
    // ORIG 0x81a3ef0：cmpw rank + js（rank<0）+ cmp 16 位 size + jle（rank>=size）
    // —— 有符号 16 位比较；合并 || 以匹配 js/jle 前置 false 布局。
    if (rank >= 0 && rank < (short)m_rankList.size())
    {
        out = m_rankList[rank];
        return true;
    }
    return false;
}

// ============================================================================
// Arad_DataManager（ORIG ctor 0x8188030；位于 ARAD 命名空间）
// ============================================================================
namespace ARAD
{
// [SplitC 2026-09-11 batch3 · TU 迁移] Arad_DataManager 已迁出至 Arad_DataManager.cpp（ORIG GI 键对齐）。



#include "ARAD/dtor_Arad_DataManager.inc"


#include "ARAD/Load.inc"


#include "ARAD/Destroy.inc"


#include "ARAD/initAradData.inc"


#include "ARAD/destroyAradData.inc"


#include "ARAD/__insertGameScript.inc"


#include "ARAD/findGameScript.inc"
{
    // ORIG 0x81882ac：return m_scriptMap[name]（未命中插入空指针，返回 0）。
    return m_scriptMap[name];
}

// Singleton<T> 模板定义已在 Arad_DataManager.h 提供。ORIG 对这两个实例只发射
// { Singleton() C1/C2, Get() }（_ZN4ARAD9SingletonINS_16Arad_DataManagerE... /
// ...INS_13ScriptManagerE...），**没有** GetPtr ⇒ 整类显式实例化会连带发出 ORIG
// 不存在的 W _ZN4ARAD9SingletonI...E6GetPtrEv，故降级为成员级显式实例化。
// 唯一定义点在 Game/Arad_Singletons.cpp；此处仅补发本 TU 用到的两个成员。
template Singleton<Arad_DataManager>::Singleton();
template Arad_DataManager* Singleton<Arad_DataManager>::Get();
template Singleton<ScriptManager>::Singleton();
template ScriptManager* Singleton<ScriptManager>::Get();

#include "ARAD/notifyOpenMessageDialog.inc"


}  // namespace ARAD
