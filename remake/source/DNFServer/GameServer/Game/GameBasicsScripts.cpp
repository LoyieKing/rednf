// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GameBasicsScripts/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include <cstring>
// [T2-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit（ORIG 实有符号），
// 使本 TU 发射 _GLOBAL__I_（键名见下 ClearData 前移注释）。
#include <iostream>
#include "CDataManager.h"
#include "GamePODTypes.h"

namespace APSystem
{
// [T1-GI · P327] ClearData 前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN8APSystem24CActionPointEtcParameter9ClearDataEv`（ORIG 0x08895652，
// ORIG CU localchina_RDARScriptActionPointSystem.cpp）。仅移函数定义，零语义/布局改动。
#include "GameBasicsScripts/ClearData.inc"
}

// ORIG C1 0x0889484a：仅按序构造 6 个容器成员，不调用 ClearData。
#include "GameBasicsScripts/CActionPointEtcParameter.inc"
}

#include "GameBasicsScripts/dtor_CActionPointEtcParameter.inc"

#include "GameBasicsScripts/IsValidTodayRewardItem.inc"
}
}


#include "GameBasicsScripts/init.inc"
}

#include "GameBasicsScripts/init_1.inc"
}


// ============================================================================
// PvPSkillTreeParameterScript 查询（ORIG 0x08a5e178 / 0x08a5dd62, 符号 T）
// 最小实现保证符号 T + 可链接；完整脚本解析逻辑待细化。
// ============================================================================
#include "GameBasicsScripts/getPvPSkillPoint.inc"
}

#include "GameBasicsScripts/getGiveSkill.inc"
}

// 全局脚本重复加载开关（ORIG 0x0949b284 B，char）
char g_isAllowScriptDuplication = 0;

// ============================================================================
// PvPSkillTreeParameterScript::getPvPSkillTree —— ORIG 0x08a5db5a（0x119 字节）
// 报告：docs/class_func_reports/PvPSkillTreeParameterScript/getPvPSkillTree.md
// key=(uchar job, uchar firstGrow, uchar secondGrow) 查 m_map0；命中后内层按
// grade 查找；命中时 *out = flag ? &value.m_vec0c(+0x0c) : &value.m_vec00(+0x00)
// 并返回 true，否则返回 false。
// ============================================================================
#include "GameBasicsScripts/getPvPSkillTree.inc"
}
