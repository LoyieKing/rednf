// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ComboSkill/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— ComboSkill（G1-2 技能域，组合技）
// 参照 docs/class_func_reports/ComboSkill.md + ORIG 反汇编。
// 布局：+0x00 std::map<short, vector<int>> m_comboSkillMap（0x15）
//       +0x15 std::map<short, vector<int>> m_comboSkillSfpMap（0x15）
//       sizeof = 0x2a；SkillSlot 内嵌于 +0x1c。
// 默认构造从 G_CDataManager()+0x14 指向的 vector<pair<int,int>>（+0x48fc）
// 预填 6 组组合技映射；saveData/loadData 按 SIG_COMBO_SKILL 的 pack(1) 布局
// 在 +0x0a/+0x5e 存取两棵树（comoboSkillData 0xe 字节/条）。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN10ComboSkill9resetTreeE20ENUM_SKILL_TREE_KIND`（ORIG 0x80ea3c7）。
#include <iostream>


#include <utility>

#include "SkillSlot.h"
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "PacketGuard.h"



// G_CDataManager() 返回对象 +0x14 处为 char*（组合技列表基址），成员访问形态
// 与 ORIG 一致（mov 0x14(%eax),%eax；而非 add+mov）。
struct CDataManagerComboLayout
{
    char m_pad[0x14];
    char* m_pComboList;  // +0x14
};

#include "ComboSkill/resetTree.inc"
}

#include "ComboSkill/ComboSkill.inc"
}

#include "ComboSkill/ComboSkill_1.inc"
}

#include "ComboSkill/dtor_ComboSkill.inc"
}

ComboSkill& ComboSkill::operator=(const ComboSkill& other)
{
    m_comboSkillMap = other.m_comboSkillMap;
    m_comboSkillSfpMap = other.m_comboSkillSfpMap;
    return *this;
}

#include "ComboSkill/convertComboSkillMap.inc"
}

#include "ComboSkill/convertComboSkillArray.inc"
}

#include "ComboSkill/loadData.inc"
}

#include "ComboSkill/saveData.inc"
}

#include "ComboSkill/getComboSkillSendList.inc"
}

#include "ComboSkill/getComboSkillSendForm.inc"
}

#include "ComboSkill/setComboSkill.inc"
}

#include "ComboSkill/eraseDuplicationComboSKill.inc"
}
