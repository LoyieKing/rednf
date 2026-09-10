// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StrikerWorldDropB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// StrikerWorldDropB2.cpp — Striker/WorldDrop/StageMap 依赖恢复（独占 TU，
// 任务 StrikerWorldDropB2）
//
// 覆盖符号（全部经 ORIG df_game_r nm 核实；报告存在者已完整阅读 ##汇编 +
// ##反编译 C 后转写，无函数报告两处 objdump 逐指令转写并标注）：
//   StrikerScript::importSkillDataUpVector()                        T 0x08a9e4d6
//   StrikerScript::importSkillAttackPenalty()                       T 0x08a9e5d0
//   StrikerScript::importSkillCoolTimePenalty()                     T 0x08a9e5fc
//   StrikerScript::importParentLevelPenlaty()                       T 0x08a9e654
//   StrikerScript::readSkillPenalty(std::vector<TagSkillPenalty>&)  T 0x08a9e6cc
//       （importSkillAttackPenalty/importSkillCoolTimePenalty 的直接依赖；
//        此前全仓无定义，随本批闭环）
//   StrikerSkill::StrikerSkill()                                    W 0x08a9ed1e
//       （无独立函数报告【标记】：先读同类宿主报告 StrikerScript/
//        StrikerScript.md 与 readSkillPenalty.md，再 objdump 转写：
//        仅构造 +0xc std::string，int 字段不初始化）
//   WongWork::CWorldDrop::CWorldDrop(int)                           W 0x0853a826
//       （无独立函数报告【标记】：先读同类报告 WongWork__CWorldDrop/
//        loadScript.md、generateRandomItem.md 域注释与 CItemGenerator.h
//        布局证据，再 objdump 转写：调基类 C1 → 写 vptr 0x08c9a718 →
//        于 +0x18 / +0xfcc 各循环 WorldDropInfoC1Ev ×0xc8）
//   WongWork::CWorldDrop::loadScript(char const*)                   W 0x0853a81c
//       （无独立函数报告【标记】：objdump 显示纯 return 0 —— ORIG 即如此，
//        虚槽 +0x04 覆写占位；真实装载走非虚双参重载）
//   WongWork::CWorldDrop::loadScript(char const*, char const*)      T 0x0853855c
//   StageMapList::makeStageMapUniqueId(int, int, int)               T 0x08364818
//   AvatarRoulette::swap(AvatarRoulette*)                           T 0x08911e4e
//
// 报告路径（##汇编 + ##反编译 C 全文已读）：
//   docs/class_func_reports/StrikerScript/{importSkillDataUpVector,
//       importSkillAttackPenalty,importSkillCoolTimePenalty,
//       importParentLevelPenlaty,readSkillPenalty,StrikerScript}.md
//   docs/class_func_reports/AvatarRoulette/swap.md
//   docs/class_func_reports/WongWork__CWorldDrop/loadScript.md
//   docs/class_func_reports/StageMapList/makeStageMapUniqueId.md
//   docs/class_func_reports/TagSkillPenalty/TagSkillPenalty.md（元素类型同类）
//
// 不含（他人已落盘，查重后跳过）：
//   StrikerScript::{ctor,dtor,getInsance,clear,create,destory} CtorProducers_2.cpp
//   StrikerScript::importScript                                 ScriptLeafA2.cpp
//   AvatarRoulette::{C1,D1,clear}                          InitScriptProducers_6.cpp
//   STCharacterScript C1/D1（ScriptDepsA 已负责落盘；头内 inline 构造 + Clear）
// ============================================================================

#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"             // StageMapList 权威声明
#include "DataManagerInitDepends.h"   // AvatarRoulette 权威布局（须在 CDataManager.h 之后）
#include "StrikerScript.h"            // StrikerScript / StrikerSkill / 子类型
#include "EquipmentParameterInfo.h"   // importSkillDataUp + EquipmentSkillDataUp
#include "DNFLexWrapper.h"            // ScanType / ScanInt / ScanFloat / ScanStr
#include "CItemGenerator.h"           // WongWork::CWorldDrop

// [GI 调序 T1a] TagSkillPenalty ctor 外联定义（ORIG T 0x08a9d768：仅 call clear，体 0x13）。
// 置于本 TU 首个强符号位 ⇒ GI 键名 = `_GLOBAL__I__ZN15TagSkillPenaltyC2Ev`（ORIG 0x08a9eced，
// ORIG CU = RDARScriptStriker.cpp，即本 TU 对应物）。判据 P327：仅外联强符号参与键名。
#include "StrikerWorldDropB2/TagSkillPenalty.inc"
}

// ============================================================================
// StrikerScript 脚本导入叶子（报告全文转写）
// ============================================================================

// ORIG T 0x08a9e4d6：clear this+0x40；顶测循环两段条件各自独立判退：
// ScanType(token,true)!=true → 退出；token!="[level]" → 退出；
// 循环体 { StrikerSkillDataUp 局部(0x14)；+0=ScanInt、+4=ScanInt；
// importSkillDataUp(&局部+8)；push_back }。for(;;)+break 形态与 ORIG 的
// 「条件在顶部、无前置 jmp、两个 break 各自跳出」布局同构。
#include "StrikerWorldDropB2/importSkillDataUpVector.inc"
}

// ORIG T 0x08a9e5d0：clear this+0x4c 后直呼 readSkillPenalty(this+0x4c)。
#include "StrikerWorldDropB2/importSkillAttackPenalty.inc"
}

// ORIG T 0x08a9e5fc：clear this+0x58 后直呼 readSkillPenalty(this+0x58)。
#include "StrikerWorldDropB2/importSkillCoolTimePenalty.inc"
}

// ORIG T 0x08a9e654：clear this+0x6c；ok=true 置一次；每轮首值 ScanInt(&ok)
// 失败即停；随后 ScanInt / ScanFloat×2 凑成 ParentLevelPenalty(2×int+2×float)
// push_back。ORIG 栈对象无 ctor 调用（POD 直接逐字段写入）。
#include "StrikerWorldDropB2/importParentLevelPenlaty.inc"
}

// ORIG T 0x08a9e6cc：死循环 { ok=true（循环体首指令）；TagSkillPenalty 局部
// C1（=clear，报告 TagSkillPenalty.md）；+0=ScanInt(&ok) 失败即停；
// +4=ScanInt；ScanStr 后与 "%" 比较 → +8 bool；+0xc=ScanInt；push_back }。
#include "StrikerWorldDropB2/readSkillPenalty.inc"
}

// ============================================================================
// StrikerSkill 生命周期（无独立函数报告【标记】，objdump 逐指令转写）
// ============================================================================

// ORIG W 0x08a9ed1e：仅 call _ZNSsC1Ev(this+0xc)；三个 int 字段保持未初始化。
#include "StrikerWorldDropB2/StrikerSkill.inc"
#include "StrikerWorldDropB2/CWorldDrop.inc"
#include "StrikerWorldDropB2/loadScript.inc"
}

// ORIG T 0x0853855c（报告转写）：ImportWorldDrop(this+0x18, path) 失败返回
// 0x7b9；ImportWorldDrop(this+0xfcc, warArea) 失败返回 0x7bb；成功返回 0。
// 注：DataManagerInitDepends.h 既存声明第二参为 char*（ORIG mangled 实为
// PKc），属 InitScriptProducers_1 域既存 ABI 偏差，此处 const_cast 桥接。
#include "StrikerWorldDropB2/loadScript_1.inc"
}

// ============================================================================
// ORIG T 0x08364818：三参在栈上分别 +100/+1000/+1000（addl 直改栈槽）后
// imul $0x5f5e100 / imul $0x2710 / add 线性组合。保持同构写法以对齐指令形态。
#include "StrikerWorldDropB2/makeStageMapUniqueId.inc"
}
// ============================================================================
// AvatarRoulette::swap（报告全文转写；生命周期其余部分在 InitScriptProducers_6）
// ============================================================================

// ORIG T 0x08911e4e：前 5 个 dword 自 other 单向拷贝（+0x00..+0x10），
// 随后 i=0..10 对 m_avatars[i]（步长 0xc @+0x14）调 vector::swap（双向），
// 恒返回 1。stiker 数组不参与。
#include "StrikerWorldDropB2/swap.inc"
}
