// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CSkill/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 技能定义类 CSkill 还原（G1 玩家域第一批）。
// 逐函数对照 docs/class_func_reports/CSkill.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
#include <string.h>

#include "CSkill.h"
#include "CDataManager.h"

// ---- STSecondGrowTypeFitness（ORIG 0x083748da / 0x083748ee）----
#include "CSkill/clear.inc"
}

#include "CSkill/STSecondGrowTypeFitness.inc"
}

// ---- 构造 / 析构（ORIG 0x08374944 / 0x08379fd8）----
#include "CSkill/CSkill.inc"
}

#include "CSkill/dtor_CSkill.inc"
}

// ORIG 0x83507e8：组号重排（flag → 3；0..3 按 count 改 1/0；4 → 2）
#include "CSkill/reform_ui_group_no.inc"
}

// ---- 简单访问器（ORIG weak inline，单条指令形态）----
#include "CSkill/get_index.inc"
}

#include "CSkill/get_group.inc"
}

#include "CSkill/set_index.inc"
}

#include "CSkill/set_group.inc"
}

#include "CSkill/set_job_type.inc"
}

#include "CSkill/set_skill_type.inc"
}

#include "CSkill/set_level_limit.inc"
}

#include "CSkill/SetStealable.inc"
}

#include "CSkill/SetStealLevelPenalty.inc"
}

#include "CSkill/SetStealSPPenalty.inc"
}

#include "CSkill/GetStealLevelPenalty.inc"
}

#include "CSkill/GetStealSPPenalty.inc"
}

#include "CSkill/set_fitGrowType.inc"
}

#include "CSkill/set_fitness.inc"
}

#include "CSkill/get_fitness.inc"
}

#include "CSkill/set_type_max_level.inc"
}

#include "CSkill/get_type_max_level.inc"
}

#include "CSkill/set_fitness_second_growtype.inc"
}

#include "CSkill/get_fitness_second_growtype.inc"
}

#include "CSkill/set_second_growtype_fitness.inc"
}

#include "CSkill/get_second_growtype_fitness.inc"
}

#include "CSkill/set_second_growtype_max_level.inc"
}

#include "CSkill/get_second_growtype_max_level.inc"
}

#include "CSkill/set_prelearned_skill.inc"
}

#include "CSkill/add_need_sp.inc"
}

#include "CSkill/add_need_sfp.inc"
}

#include "CSkill/add_learned_skill.inc"
}

#include "CSkill/setSkillName.inc"
}

#include "CSkill/getSkillName.inc"
}

#include "CSkill/is_active_skill.inc"
}

#include "CSkill/IsSpecialSkill.inc"
}

#include "CSkill/isCommandCustomizing.inc"
}

// ---- 主逻辑（ORIG strong）----
// ORIG 0x83504ec _ZNK6CSkill11IsStealableEii：体内 `xor $0x1,%eax; test %al,%al` bool 物化形
#include "CSkill/IsStealable.inc"
}

#include "CSkill/can_learn.inc"
}

#include "CSkill/getMaxSkillLevel.inc"
}

#include "CSkill/get_need_sp_for_level.inc"
}

#include "CSkill/get_need_sfp_for_level.inc"
}

#include "CSkill/get_need_sp_sfp_for_level.inc"
}

#include "CSkill/get_guildSkillNeedGSP.inc"
}

#include "CSkill/get_guildSkillNeedGold.inc"
}

#include "CSkill/get_needGuildLevel.inc"
}
