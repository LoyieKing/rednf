// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DisJoint/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// DisJoint.cpp — DisJoint 查询方法实现（本批次独占文件）
//
// 依据 docs/class_func_reports/DisJoint/GetAddtionalSkill.md（## 汇编 +
// ## 反编译 C 全文读取后转写）：
//   DisJoint::GetAddtionalSkill(int,int,int&,int&)  T 0x08473184
//
// [skill info] 行元素语义来自 importDisjointScript（ScriptDepsA.cpp 实证）：
//   m_field00 = getCharacterJob(str)  职业
//   m_field04 = ScanInt()             技能等级
//   m_field08 = ScanInt()             附加技能索引
//   m_field0c = getElement(str)       附加技能元素/等级
// 其余 DisJoint 方法（GetResult 由 ItemUpgradeVtableB2.cpp 交付；
// GetPredictResult 由 ExpertJobLink4.cpp 交付；LoadDisJoint 待脚本导入域批次）
// ——本 TU 补 GetAddtionalSkill 与 _GetAdditionalItem/_GetAdditionalItemInfo
// （自 ItemUpgradeVtableB2.cpp 迁入，按真实成员布局重写）。
// ============================================================================
// [b28d-64 判词汇总 · 泳道64] 本 TU 1 件：_GetAdditionalItem 0x084732b4
//   103/104 N封顶（P167 CLEAR；残余 = N1 折叠 add $0x150 vs $0x154 + bool
//   局部落槽 vs ORIG `test %al;je` 寄存器形）。

#include "DisJoint.h"
#include "CItem.h"

#include "DisJoint/get_rand_int.inc"
#include "DisJoint/GetAddtionalSkill.inc"
}

// ============================================================================
// DisJoint::_GetAdditionalItemInfo @ 0x08473224
//   m_additional[rarity]（+0x5c8+rarity*0x10）m_count==0 → 0；
//   否则 idx = get_rand_int(m_count*1000)/1000，返回 m_items[idx]。
//   （旧裸偏移转写迁出 ItemUpgradeVtableB2.cpp，改按 DisJoint.h 真实成员访问；
//   保留 ORIG 的 result(-0x10)/bound(-0xc) 槽结构，pick 内联进 operator[]。）
// ============================================================================
#include "DisJoint/_GetAdditionalItemInfo.inc"
}

// ============================================================================
// DisJoint::_GetAdditionalItem @ 0x084732b4（ORIG mangled ...EPK5CItemRiS3_：
//   outIdx/outCount 为 int& 引用参数，非指针）
//   outIdx = _GetAdditionalItemInfo(item->get_rarity())；非零时按
//   m_additionalConst[rarity].m_item*100 权重二选一分母浮点
//   （m_additionalWeight / m_additionalRate[rarity]），
//   outCount = (int)(grade / denom)（ORIG fistpl 前 fldcw 置 RC=truncate，
//   截断非四舍五入），最小 1。
//   分支内 get_grade/get_rarity 各自独立再调用（O0 无 CSE，P48）。
// ============================================================================
#include "DisJoint/_GetAdditionalItem.inc"
}
