// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CharacManageScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CharacManageScript.cpp —— charac_manage.etc 脚本宿主查询方法（ORIG 专属批次）
//
// 覆盖 5 个 ORIG 符号（均经 docs/class_func_reports/CharacManageScript/ 报告
// ## 汇编 + ## 反编译 C 完整读取后转写）：
//   GetExpBonus            T 0x088e0f58（GetExpBonus.md）
//   GetGoldBonus           T 0x088e0fce（GetGoldBonus.md）
//   GetLevel               T 0x088e0ed4（GetLevel.md）
//   GetPoint               T 0x088e0e0a（GetPoint.md）
//   GetLevelUpRequirePoint T 0x088e1114（GetLevelUpRequirePoint.md）
//
// 此前实现分别暂居 CPartyDepsD2.cpp（GetExpBonus/GetGoldBonus，PartyDepsD2
// 批次依赖闭环）与 CUser.cpp（GetLevel/GetPoint/GetLevelUpRequirePoint，
// LinkMisc5 批次），本批次归位至本 TU。
// ============================================================================

#include "CharacManageScript.h"

// ---------------------------------------------------------------------------
// CharacManageScript::GetExpBonus —— ORIG T 0x088e0f58
// m_expBonus(+0x18)[level] 命中 → fildl 值后 flds _DAT_08e0c554(=0.1f，
// rodata 8e0c554: 3d cc cc cd) fdivrp → 值/0.1f；未命中 fldz。
// （GetGoldBonus 同构，map 为 m_goldBonus +0x30。）
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetExpBonus.inc"
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetGoldBonus —— ORIG T 0x088e0fce（同 GetExpBonus 构）
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetGoldBonus.inc"
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetLevel —— ORIG T 0x088e0ed4
// point 为 0 返 0；自高向低找首个不高于 point 的门槛，等级取其下标加一，
// 最后 clamp 到 m_manageLevelMax。
// b28c-28：cursor 声明 int（非 unsigned）⇒ 4.4 复现 ORIG 实参求值序
// `mov slot,%eax; mov this,%edx; add $0xc,%edx`（探针 /tmp/l28_probe2.cpp fD/fE）。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetLevel.inc"
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetPoint —— ORIG T 0x088e0e0a
// Σ 每行区间 [start..end] 内 lv 不超过 level 的 bonus。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetPoint.inc"
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetLevelUpRequirePoint —— ORIG T 0x088e1114
// max 为 0 或越界返 0；否则取第 max 个门槛值。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetLevelUpRequirePoint.inc"
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetExpBonus(int, int*) —— ORIG T 0x088e1044
// 命中 m_expBonus[level] 写 *out，未命中不动 *out（void 返回）。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetExpBonus_1.inc"
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetGoldBonus(int, int*) —— ORIG T 0x088e10ac（同 GetExpBonus 构）
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetGoldBonus_1.inc"
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetMinimumPointCharacterLevel —— ORIG T 0x088e1266
// m_pointBonus 空 → 0x28；否则取首行 m_field00。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetMinimumPointCharacterLevel.inc"
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetName —— ORIG T 0x088e1166
// m_basicExplain[key] 命中 → 拷贝 string，未命中 → 空串（std::string 返回值）。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetName.inc"
}

// ---------------------------------------------------------------------------
// CharacManageScript::GetOptionValue —— ORIG T 0x088e11fc
// m_manageOption[type] 命中 → second，未命中 → 0。
// ---------------------------------------------------------------------------
#include "CharacManageScript/GetOptionValue.inc"
}
