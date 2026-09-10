// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StScriptImporters/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// getGrowTypeIndex 定义于 InitScriptProducers_6.cpp（ORIG T 0x088be63d）；无权威头，
// 照 InitScriptProducers_7.cpp 局部声明模式。
#include "StScriptImporters/getGrowTypeIndex.inc"
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

// ORIG C1 0x891ba22（0x48）：vector<int>(+0x8) C1 + clear()。
#include "StScriptImporters/stRecommendQPData.inc"
}

// ORIG D1 0x891ba6a（0x16）：仅 vector<int> 析构。
#include "StScriptImporters/dtor_stRecommendQPData.inc"
}

// ORIG C1ERKS_ 0x892f730（0x36）：两 int + vector<int> 复制。
#include "StScriptImporters/stRecommendQPData_1.inc"
}

// ORIG op= 0x892f7ea（0x39）：两 int + vector<int> 赋值。
stQPGuideScript::stRecommendQPData& stQPGuideScript::stRecommendQPData::operator=(
    stRecommendQPData const& other)
{
    m_job = other.m_job;
    m_grow = other.m_grow;
    m_items = other.m_items;
    return *this;
}

// ORIG clear 0x891ba80（0x3f）：+0x00/+0x04 int=-1、+0x08 vector clear + reserve(4)。
#include "StScriptImporters/clear.inc"
}

// ============================================================================
// stPartyMatchingInfo（报告缺失，objdump 直转）
// ============================================================================

// ORIG C1 0x891b78e（0x8e）：两 string + vector<int> 构造后 clear()。
#include "StScriptImporters/stPartyMatchingInfo.inc"
}

// ORIG D1 0x891ca12（0x6d）：vector + 两 string 逆序析构。
#include "StScriptImporters/dtor_stPartyMatchingInfo.inc"
}

// ORIG C1ERKS_ 0x892ead6（0x86）：两 string + vector<int> 复制。
#include "StScriptImporters/stPartyMatchingInfo_1.inc"
}

// ORIG op= 0x892ebe0（0x4d）：string op= ×2 + vector op=。
stPartyMatchingInfo& stPartyMatchingInfo::operator=(stPartyMatchingInfo const& other)
{
    m_channelName = other.m_channelName;
    m_regionName = other.m_regionName;
    m_dungeonIdx = other.m_dungeonIdx;
    return *this;
}

// ORIG clear 0x891b81c（0x2f）：两 string clear + vector<int> clear。
#include "StScriptImporters/clear_1.inc"
}

// ============================================================================
// stNeoplePCBANG（报告缺失，objdump 直转）
// ============================================================================

// ORIG C1 0x891acce（0x48）：vector<pair>(+0xc) C1 + clear()。
#include "StScriptImporters/stNeoplePCBANG.inc"
}

// ORIG D1 0x891c6c2（0x16）：仅 vector<pair> 析构。
#include "StScriptImporters/dtor_stNeoplePCBANG.inc"
}

// ORIG C1ERKS_ 0x89276f8（0x42）：三 int + vector<pair> 复制。
#include "StScriptImporters/stNeoplePCBANG_1.inc"
}

// ORIG op= 0x89277be（0x45）：三 int + vector<pair> 赋值。
stNeoplePCBANG& stNeoplePCBANG::operator=(stNeoplePCBANG const& other)
{
    m_field0 = other.m_field0;
    m_level = other.m_level;
    m_peopleCount = other.m_peopleCount;
    m_status = other.m_status;
    return *this;
}

// ORIG clear 0x891ac9a（0x33）：+0x00/+0x04/+0x08 int=0 + vector<pair> clear。
#include "StScriptImporters/clear_2.inc"
}

// ============================================================================
// stBloodWarBgmTable（报告缺失，objdump 直转）
// ============================================================================

// ORIG C1 0x891a95c（0x48）：string(+0x4) C1 + clear()。
#include "StScriptImporters/stBloodWarBgmTable.inc"
}

// ORIG D1 0x891c9fc（0x16）：仅 string 析构。
#include "StScriptImporters/dtor_stBloodWarBgmTable.inc"
}

// ORIG C1ERKS_ 0x892d3b6（0x2a）：int + string 复制。
#include "StScriptImporters/stBloodWarBgmTable_1.inc"
}

// ORIG op= 0x892d464（0x2d）：int + string 赋值。
stBloodWarBgmTable& stBloodWarBgmTable::operator=(stBloodWarBgmTable const& other)
{
    m_id = other.m_id;
    m_name = other.m_name;
    return *this;
}

// ORIG clear 0x891a9a4（0x1f）：+0x00 int=-1 + string clear。
#include "StScriptImporters/clear_3.inc"
}

// ============================================================================
// stBloodPoint / stUltimateWarMonsterInfo（importBloodWarEtcScript 依赖，报告缺失）
// ============================================================================

// ORIG C1 0x891a9c4（0x13）= clear()；D1 0x893f604 trivial（5B）。
#include "StScriptImporters/stBloodPoint.inc"
}

// ORIG clear 0x891a9d8（0x2c）：+0x00=4、+0x04/+0x08/+0x0c=0。
#include "StScriptImporters/clear_4.inc"
}

// ORIG C1 0x891aa04（0x13）= clear()；D1 0x893f858 trivial（5B）。
#include "StScriptImporters/stUltimateWarMonsterInfo.inc"
}

// ORIG clear 0x891aa18（0x22）：三 int=0。
#include "StScriptImporters/clear_5.inc"
}

// ============================================================================
// importQPGuideScript —— ORIG T 0x0890ff93（报告全文转写）
// ============================================================================
#include "StScriptImporters/importQPGuideScript.inc"
}

// ============================================================================
// importPartyMatchingInfo —— ORIG T 0x0890f311（报告全文转写）
// ============================================================================
#include "StScriptImporters/importPartyMatchingInfo.inc"
}

// ============================================================================
// importPCBANGScript —— ORIG T 0x08905773（报告全文转写）
// ============================================================================
#include "StScriptImporters/importPCBANGScript.inc"
}

// ============================================================================
// importBloodWarEtcScript —— ORIG T 0x0890e3f8（报告全文转写）
// ============================================================================
#include "StScriptImporters/importBloodWarEtcScript.inc"
}
