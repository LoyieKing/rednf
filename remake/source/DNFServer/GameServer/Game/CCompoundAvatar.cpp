// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CCompoundAvatar/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CCompoundAvatar.cpp — 头像合成/分离脚本域生命周期与查询（Ctor2_ContB）
//
// 全部实现按 docs/class_func_reports/{WongWork__CCompoundAvatar,
// tagCompoundAvatarTable}/ 报告 + ORIG objdump 逐段转写：
//   STAncientRareAvatarCube C1 W 0x0891aa3a（copy/D1 由编译器生成，与 ORIG W 等价）
//   tagCompoundAvatar       Clean W 0x0891ab56（特member 函数由编译器生成，
//                           逐成员顺序与 ORIG W 0x0891be88/0x0891c1f8/
//                           0x0833463c/0x0894ec3a 一致）
//   tagCompoundAvatarTable C1 W 0x0833431c / D1 W 0x08334330 /
//                          getCompoundAvatarInfo W 0x08903fcc
//   WongWork::CCompoundAvatar C1 T 0x08331d74 / D1 T 0x08331ea2 /
//                          LoadScript T 0x08333ea6
// importCompoundAvatarTable(T 0x08903ea7) 仅声明——定义归脚本导入域批次。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射。
// [T1-GI · P327] 键名 `_GLOBAL__I__ZN8WongWork15CCompoundAvatarC2Ev`（ORIG 0x8334202）：
// 4.4 实测首强符号 = STAncientRareAvatarCube C1（**类外定义 ⇒ 强符号 T**，非弱；
// 其 W 仅出现于编译器的 copy/D1 生成件）⇒ WongWork::CCompoundAvatar C1 定义必须
// 前移到它之前，见下方「键名位」块。
#include <iostream>

#include "CCompoundAvatar.h"
#include "PvPTypes.h"      // CMTRand 完整类型（0x9c8）

#include <ctime>
#include <cstring>

// ==================== WongWork::CCompoundAvatar（键名位 · P327）====================
// ORIG 0x08331d74：11×tagCompoundAvatarTable 构造（成员数组自动）→
// CMTRand(new 0x9c8)(time(0)) 存 +0x84 → 100 次 randInt(10000) 热身。
// 槽残差：ORIG seed=-0x24/range=-0x20 vs 4.4 call 临时恒浅槽对调（P212 全头部
// 声明试形反劣 8→10，具名 seed 亦不落深槽）——与 CItemUpgrade_Separate::C1
// b26 判词同型，N82 族封顶。
namespace WongWork
{
#include "CCompoundAvatar/CCompoundAvatar.inc"
}
}  // namespace WongWork

// ============================ STAncientRareAvatarCube ============================

// ORIG 0x0891aa3a：单循环内逐组「置零 int + clear 向量」，末尾 clear 首向量。
#include "CCompoundAvatar/STAncientRareAvatarCube.inc"
}

// ============================ tagCompoundAvatar ============================

// ORIG 0x0891ab56：标量清零（含 memset 0x28 于 +0x04）+ 三组/尾向量逐个 clear，
// 循环体内顺序为 A.clear/B.clear/m_b/m_c/m_d/C.clear，尾部顺序 2b8/2dc/2e0[]/2c4/2d0。
#include "CCompoundAvatar/Clean.inc"
}

// ============================ tagCompoundAvatarTable ============================

#include "CCompoundAvatar/tagCompoundAvatarTable.inc"

#include "CCompoundAvatar/dtor_tagCompoundAvatarTable.inc"
#include "CCompoundAvatar/getCompoundAvatarInfo.inc"
}

// ============================ WongWork::CCompoundAvatar ============================

namespace WongWork
{

// ORIG 0x08331ea2：delete m_rand 并置空；11 张表逆序析构（编译器生成）。
#include "CCompoundAvatar/dtor_CCompoundAvatar.inc"
}

// ORIG 0x08333ea6：importCompoundAvatarTable(dir,path,this) == 0。
#include "CCompoundAvatar/LoadScript.inc"
}

}  // namespace WongWork
