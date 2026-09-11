#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StScriptImporters/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// StScriptImporters.cpp —— st* 脚本结构 W 符号实现 + 全局导入函数（OrigStStructs 独占）。
//
// 覆盖 ORIG 真实符号（nm 实证均为 W = 头内联强度，本 TU 以类外定义提供同名符号）：
//   stQPGuideScript::stRecommendQPData  C1 0x891ba22 / C1ERKS_ 0x892f730 / D1 0x891ba6a
//                                      op= 0x892f7ea / clear 0x891ba80
//   stPartyMatchingInfo                 C1 0x891b78e / C1ERKS_ 0x892ead6 / D1 0x891ca12
//                                      op= 0x892ebe0 / clear 0x891b81c
//   stNeoplePCBANG                      C1 0x891acce / C1ERKS_ 0x89276f8 / D1 0x891c6c2
//                                      op= 0x89277be / clear 0x891ac9a
//   stBloodWarBgmTable                  C1 0x891a95c / C1ERKS_ 0x892d3b6 / D1 0x891c9fc
//                                      op= 0x892d464 / clear 0x891a9a4
//   stBloodPoint                        C1 0x891a9c4 / clear 0x891a9d8（importBloodWarEtcScript 依赖）
//   stUltimateWarMonsterInfo            C1 0x891aa04 / clear 0x891aa18（同上）
// 以及 ORIG T 导入函数：
//   importQPGuideScript    0x0890ff93 / importPartyMatchingInfo 0x0890f311
//   importPCBANGScript     0x08905773 / importBloodWarEtcScript 0x0890e3f8
//
// 报告：docs/class_func_reports/_lt_global_gt_/{importQPGuideScript,importPartyMatchingInfo,
//       importPCBANGScript,importBloodWarEtcScript}.md；W 符号无报告，objdump 直转
//       （汇编已逐条核对：clear/C1/C1ERKS_/D1/op= 各成员序与 ORIG 一致）。

#include <utility>
#include <vector>

#include "stQPGuideScript.h"
#include "stPartyMatchingInfo.h"
#include "stNeoplePCBANG.h"
#include "stBloodWarBgmTable.h"

#include "NpcScript.h"          // loadRDARScriptFile / getCharacterJob
#include "DNFLexWrapper.h"      // ScanType / ScanStr / ScanInt

#include "StScriptImporters/getGrowTypeIndex.inc"

// stBloodWarGrowTable / stBloodWarBloodPointTable —— importBloodWarEtcScript 形参类型
// （mangled P19stBloodWarGrowTable / P25stBloodWarBloodPointTable）。GCC 4.4 对
// typedef 到模板实例会展开 typedef 名，唯派生结构体才能产生 ORIG mangled。
// stBloodWarGrowTable 亦定义于 WarRoom.cpp（ImportWarRoomWPTable 形参，布局一致；
// 纯派生 struct 无成员函数，链接无符号冲突）。此处为 importBloodWarEtcScript
// 提供本 TU 视图。
struct stBloodWarGrowTable
    : public std::vector<std::vector<std::pair<int, int> > >
{
};
struct stBloodWarBloodPointTable
    : public std::vector<stBloodPoint>
{
};

// ============================================================================
// stQPGuideScript::stRecommendQPData（报告缺失，objdump 直转）
// ============================================================================

#include "StScriptImporters/stRecommendQPData.inc"


#include "StScriptImporters/dtor_stRecommendQPData.inc"


#include "StScriptImporters/stRecommendQPData_1.inc"


// ORIG op= 0x892f7ea（0x39）：两 int + vector<int> 赋值。
stQPGuideScript::stRecommendQPData& stQPGuideScript::stRecommendQPData::operator=(
    stRecommendQPData const& other)
{
    m_job = other.m_job;
    m_grow = other.m_grow;
    m_items = other.m_items;
    return *this;
}

#include "StScriptImporters/clear.inc"


// ============================================================================
// stPartyMatchingInfo（报告缺失，objdump 直转）
// ============================================================================

#include "StScriptImporters/stPartyMatchingInfo.inc"


#include "StScriptImporters/dtor_stPartyMatchingInfo.inc"


#include "StScriptImporters/stPartyMatchingInfo_1.inc"


// ORIG op= 0x892ebe0（0x4d）：string op= ×2 + vector op=。
stPartyMatchingInfo& stPartyMatchingInfo::operator=(stPartyMatchingInfo const& other)
{
    m_channelName = other.m_channelName;
    m_regionName = other.m_regionName;
    m_dungeonIdx = other.m_dungeonIdx;
    return *this;
}

#include "StScriptImporters/clear_1.inc"


// ============================================================================
// stNeoplePCBANG（报告缺失，objdump 直转）
// ============================================================================

#include "StScriptImporters/stNeoplePCBANG.inc"


#include "StScriptImporters/dtor_stNeoplePCBANG.inc"


#include "StScriptImporters/stNeoplePCBANG_1.inc"


// ORIG op= 0x89277be（0x45）：三 int + vector<pair> 赋值。
stNeoplePCBANG& stNeoplePCBANG::operator=(stNeoplePCBANG const& other)
{
    m_field0 = other.m_field0;
    m_level = other.m_level;
    m_peopleCount = other.m_peopleCount;
    m_status = other.m_status;
    return *this;
}

#include "StScriptImporters/clear_2.inc"


// ============================================================================
// stBloodWarBgmTable（报告缺失，objdump 直转）
// ============================================================================

#include "StScriptImporters/stBloodWarBgmTable.inc"


#include "StScriptImporters/dtor_stBloodWarBgmTable.inc"


#include "StScriptImporters/stBloodWarBgmTable_1.inc"


// ORIG op= 0x892d464（0x2d）：int + string 赋值。
stBloodWarBgmTable& stBloodWarBgmTable::operator=(stBloodWarBgmTable const& other)
{
    m_id = other.m_id;
    m_name = other.m_name;
    return *this;
}

#include "StScriptImporters/clear_3.inc"


// ============================================================================
// stBloodPoint / stUltimateWarMonsterInfo（importBloodWarEtcScript 依赖，报告缺失）
// ============================================================================

#include "StScriptImporters/stBloodPoint.inc"


#include "StScriptImporters/clear_4.inc"


#include "StScriptImporters/stUltimateWarMonsterInfo.inc"


#include "StScriptImporters/clear_5.inc"


#include "StScriptImporters/importQPGuideScript.inc"


#include "StScriptImporters/importPartyMatchingInfo.inc"


#include "StScriptImporters/importPCBANGScript.inc"


#include "StScriptImporters/importBloodWarEtcScript.inc"

