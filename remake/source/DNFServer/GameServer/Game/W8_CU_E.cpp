// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W8_CU_E/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// W8_CU_E.cpp —— Wave8-CUser 残余 E 批：技能命令族 11 符号
// worklist /tmp/w8_batches/E_skill.txt（ORIG T/W 符号均由 nm 核对）
//
// 逐符号证据来源：
//   1. findSkillCommandData(int,ST&)      docs/class_func_reports/CUser/findSkillCommandData.md        ORIG T 0x08654b94
//   2. findSkillCommandData(int,ST&,bool) docs/class_func_reports/CUser/findSkillCommandData_08654e6a.md ORIG T 0x08654e6a
//   3. customizingSkillCommand(ST&)       docs/class_func_reports/CUser/customizingSkillCommand.md     ORIG T 0x08654f74
//   4. defaultSkillCommand()              docs/class_func_reports/CUser/defaultSkillCommand.md         ORIG T 0x086550ee
//   5. initTPSkill()                      docs/class_func_reports/CUser/initTPSkill.md                 ORIG T 0x0868fdbc
//   6. initAllSkill()                     docs/class_func_reports/CUser/initAllSkill.md                ORIG T 0x0868fc46
//   7. updateComboSkill()                 docs/class_func_reports/CUser/updateComboSkill.md            ORIG T 0x08692396
//   8. get_skill_count()                  docs/class_func_reports/CUser/get_skill_count.md             ORIG T 0x0866af86
//   9. set_skill_slot(char*,char*,int)    docs/class_func_reports/CUser/set_skill_slot.md              ORIG T 0x08662aba
//  10. st_monstorkill::Check(...)         报告缺失，objdump 直转（0x0823049c..0x08230668，W）
//  11. SIG_COMBO_SKILL::reset()           本批依赖（updateComboSkill 调用），objdump 直转
//                                         （0x08694d04..0x08694d69，W）；声明落 SigTypes.h。
// 头变更：CUser.h（st_monstorkill 注释→真实嵌套类 + initTPSkill/initAllSkill 声明）、
// SigTypes.h（SIG_COMBO_SKILL::reset 声明）；已 hub all 广播。
// ============================================================================
#include <string.h>
#include <stdlib.h>

#include "CUser.h"
#include "SigTypes.h"
#include "CDataManager.h"
#include "CSkill.h"
#include "SkillSlot.h"
#include "GameTypes.h"          // cMyTrace
#include "CGameManager.h"       // G_CGameManager()
#include "CPremiumLetheManager.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "GlobalData.h"

static_assert(sizeof(CUser::st_monstorkill) == 0x14, "st_monstorkill ABI drift");

// ---------------------------------------------------------------------------
// 1. bool CUser::findSkillCommandData(int skillId, STSkillCommandData& out)
//    @0x08654b94（findSkillCommandData.md ##汇编 + ##反编译 C 逐行转写）
#include "W8_CU_E/findSkillCommandData.inc"
}

// ---------------------------------------------------------------------------
// 2. bool CUser::findSkillCommandData(int skillId, STSkillCommandData& out, bool flag)
//    @0x08654e6a（仅查玩家命令表重载；flag 参与 Ghidra 尾参杂散，逻辑不依赖）
//    [b28d] 返回类型 int→bool（ORIG 调用点 0x081bf7d1 test %al 直判，P292；mangled 不变）。
#include "W8_CU_E/findSkillCommandData_1.inc"
}

// ---------------------------------------------------------------------------
// 3. int CUser::customizingSkillCommand(STSkillCommandData& data)
//    @0x08654f74
#include "W8_CU_E/customizingSkillCommand.inc"
}

// ---------------------------------------------------------------------------
// 4. bool CUser::defaultSkillCommand() @0x086550ee
#include "W8_CU_E/defaultSkillCommand.inc"
}

// ---------------------------------------------------------------------------
// 5. void CUser::initTPSkill() @0x0868fdbc
#include "W8_CU_E/initTPSkill.inc"
}

// ---------------------------------------------------------------------------
// 6. void CUser::initAllSkill() @0x0868fc46
#include "W8_CU_E/initAllSkill.inc"
}

// ---------------------------------------------------------------------------
// 7. void CUser::updateComboSkill() @0x08692396
#include "W8_CU_E/updateComboSkill.inc"
}

// ---------------------------------------------------------------------------
// 8. int CUser::get_skill_count() @0x0866af86
#include "W8_CU_E/get_skill_count.inc"
}

// ---------------------------------------------------------------------------
// 9. void CUser::set_skill_slot(char* data1, char* data2, int size) @0x08662aba
#include "W8_CU_E/set_skill_slot.inc"
}

// ---------------------------------------------------------------------------
// 10. bool CUser::st_monstorkill::Check(unsigned int nowTick, unsigned short nx,
//         unsigned short ny, unsigned short xLim, unsigned short yLim,
//         unsigned int windowLimit)
//     @0x0823049c size 0x1cd（objdump 直转）
#include "W8_CU_E/Check.inc"
}

// ---------------------------------------------------------------------------
// 11. void SIG_COMBO_SKILL::reset() @0x08694d04（objdump 直转；
//     ctor _ZN15SIG_COMBO_SKILLC1Ev @0x0848fbbe 写序一致）
#include "W8_CU_E/reset.inc"
}
