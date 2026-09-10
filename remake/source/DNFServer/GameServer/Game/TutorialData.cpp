// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TutorialData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// TutorialData — df_game_r ORIG 转写（docs/class_func_reports/TutorialData/*）
// 新 TU：需在 source/cmake/game/CMakeLists.txt 登记（宿主收口）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN12TutorialDataC2Ev`（ORIG 0x8ab97c5）。
#include <iostream>

#include "TutorialData.h"

#include "GlobalScriptTutorial.h"   // GlobalScriptTutorial::getData（ORIG 同 TU 域 0x08ab5xxx-0x08ab9xxx；被调前置定义使 4.4 帧收窄为 sub $0x4，与 ORIG 0x08ab971e 一致）

#include <cstddef>

static_assert(sizeof(TutorialData) == 0x148,
              "TutorialData layout mismatch vs ORIG (0x148)");
static_assert(sizeof(TutorialData::DialogData) == 8,
              "DialogData layout mismatch vs ORIG (8)");
static_assert(sizeof(TutorialData::SkillData) == 12,
              "SkillData layout mismatch vs ORIG (0xc)");
static_assert(sizeof(TutorialData::TutorialNotice) == 0x10,
              "TutorialNotice layout mismatch vs ORIG (0x10)");
static_assert(sizeof(TutorialData::ItemData) == 8,
              "ItemData layout mismatch vs ORIG (8)");

// ---- 子对象生命周期（ORIG W） ----------------------------------------

// ORIG _ZN12TutorialDataC1Ev @0x08ab7e4c：
// 容器按成员顺序构造（各数组循环展开），随后 *this=0，
// 最后对 avatarIndexMap 额外执行一次 clear()。
// [A31-GI · P327] 前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN12TutorialDataC2Ev`（ORIG 0x8ab97c5）。
#include "TutorialData/TutorialData.inc"
}

// ORIG _ZN12TutorialData10DialogDataC1Ev @0x08ab981e：
// 仅默认构造 this+0 的 std::string；m_faceIndex 不初始化。
#include "TutorialData/DialogData.inc"
}

// ORIG _ZN12TutorialData14TutorialNoticeC1Ev @0x08ab985e：
// 仅默认构造 this+4 的 std::string；其余 int 成员不初始化。
#include "TutorialData/TutorialNotice.inc"
}

// ---- 主类生命周期 -----------------------------------------------------

// ORIG _ZN12TutorialDataD1Ev @0x08ab80ee：成员逆序析构，无额外逻辑。
#include "TutorialData/dtor_TutorialData.inc"
}

// ---- dungeonIndex ------------------------------------------------------

// ORIG _ZN12TutorialData15setDungeonIndexEj @0x08ab82c4
#include "TutorialData/setDungeonIndex.inc"
}

// ORIG _ZN12TutorialData15getDungeonIndexEv @0x08ab82d2
#include "TutorialData/getDungeonIndex.inc"
}

// ORIG _ZN20GlobalScriptTutorial7getDataEi @0x08ab971e（报告 getData.md 已读）
// 越界返回 0；m_data[index].getDungeonIndex()==0 返回 0，否则返回该元素地址。
// N96 清绑：原 `register bool isZero asm("al")` 已删。ORIG isZero 无槽
// （test %eax;sete %al;test %al,%al;je）⇒ 按 P226扩展/P316① 用语句表达式包住条件。
// 清绑前后 25=25、与 ORIG 逐条全等（仅 .o 重定位地址差）⇒ 残差 0；
// 具名 bool 局部形为 26（+1 槽物化）。
#include "TutorialData/getData.inc"
}

// ---- 对话数据 ----------------------------------------------------------

// ORIG _ZN12TutorialData13addDialogDataEiiPKc @0x08ab82dc
#include "TutorialData/addDialogData.inc"
}

// ORIG _ZN12TutorialData13getDialogSizeEj @0x08ab8486
#include "TutorialData/getDialogSize.inc"
}

// ORIG _ZN12TutorialData12getDialogStrEjj @0x08ab852e
// ORIG 反汇编：map.find(0x10=param2)、vector.at(0xc=param1)、
// 越界比较 param1 < size —— 即 param1 为 vector 索引、param2 为 map key。
#include "TutorialData/getDialogStr.inc"
}

// ORIG _ZN12TutorialData12getFaceIndexEjj @0x08ab85de（参数语义同 getDialogStr）。
#include "TutorialData/getFaceIndex.inc"
}

// ORIG _ZN12TutorialData16isCurrentEpisodeEjj @0x08ab84fe
#include "TutorialData/isCurrentEpisode.inc"
}

// ---- 头像索引 ----------------------------------------------------------

// ORIG _ZN12TutorialData14addAvatarIndexEii @0x08ab868a：
// make_pair(p1,p2) 插入 this+0x1c。
#include "TutorialData/addAvatarIndex.inc"
}

// ORIG _ZN12TutorialData14getAvatarIndexE18ENUM_EQUIPMENTTYPE @0x08ab874e
#include "TutorialData/getAvatarIndex.inc"
}

// ---- 装备索引 ----------------------------------------------------------

// ORIG _ZN12TutorialData17addEquipmentIndexEiii @0x08ab86e0：
// p3<4 门控；make_pair(p1,p2) 插入 m_equipmentMaps[p3]。
#include "TutorialData/addEquipmentIndex.inc"
}

// ORIG _ZN12TutorialData17getEquipmentIndexE18ENUM_EQUIPMENTTYPEi @0x08ab87be
#include "TutorialData/getEquipmentIndex.inc"
}

// ---- 技能索引 ----------------------------------------------------------

// ORIG _ZN12TutorialData16getConvertHotKeyEi @0x08ab885c
#include "TutorialData/getConvertHotKey.inc"
}

// ORIG _ZN12TutorialData13addSkillIndexEiiiii @0x08ab8888：
// p1<4 门控；SkillData{+4:p4,+0:p3,+8:p5}（ORIG 写序）；键=getConvertHotKey(p2)；
// 键为 -1 时丢弃（按 ORIG 不释放）；插入 m_skillMaps[p1]。
#include "TutorialData/addSkillIndex.inc"
}

// ORIG _ZN12TutorialData12getSkillDataEii @0x08ab89fa：
// 用 p2 原值查 m_skillMaps[p1]（与插入的转换键不同口径，忠实转写）。
#include "TutorialData/getSkillData.inc"
}

// ORIG _ZN12TutorialData19addPassiveSkillDataEiii @0x08ab8944：
// p1<4 门控；SkillData{+4:p3,+0:p2,+8:-1}（ORIG 写序）push 入 m_passiveSkills[p1]。
#include "TutorialData/addPassiveSkillData.inc"
}

// ORIG _ZN12TutorialData19getPassiveSkillDataEi @0x08ab89a6（sret）：
// p1 越界返回空副本；命中返回成员向量副本。
#include "TutorialData/getPassiveSkillData.inc"
}

// ---- 物品 -------------------------------------------------------------

// ORIG _ZN12TutorialData7addItemEii @0x08ab8d1c
#include "TutorialData/addItem.inc"
}

// ORIG _ZN12TutorialData7getItemEv @0x08ab8d5c（sret）
#include "TutorialData/getItem.inc"
}

// ---- 教程公告 ---------------------------------------------------------

// ORIG _ZN12TutorialData17addTutorialNoticeEiiiPKc @0x08ab8a86：
// TutorialNotice{+0:p1,+4:p4 文本,+8:p2,+c:p3}；make_pair(p1, vec) 插入 m_notices。
#include "TutorialData/addTutorialNotice.inc"
}

// ORIG _ZN12TutorialData16getNoticeAniPathEi @0x08ab8c4c（sret）：
// 未命中复制空临时向量。
#include "TutorialData/getNoticeAniPath.inc"
}
