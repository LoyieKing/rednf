// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TownAreaScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// TownAreaScript（G2 Village/区域批次）——按 ORIG 反汇编真实实现。
//   C1Ev 0x86d1d9a：构造 m_mapName/m_rects/m_field18 后调 clear()。
//   D1Ev 0x86d28f6：逆序析构 m_field18/m_rects/m_mapName。
//   C1ERKS_ 0x86d296a：逐成员拷贝。
//   aSERKS_ 0x86d2a1a：逐成员赋值。
//   clear 0x86d1d46：m_areaIndex=-1、m_mapName=""、m_areaType=0、
//                    两 vector 清空、m_field24=1。
// Village.cpp / Area.cpp 共用 TownAreaScript.h（唯一声明点，sizeof 0x30）。
// ============================================================================

#include "TownAreaScript.h"

// [b28-L14] ORIG C1Ev 0x86d1d9a 只构造 m_mapName/m_rects/m_field18 后调 clear()
// （无标量清零）；C1ERKS_/aSERKS_ 为 W 隐式合成（P74），用户定义已删。
#include "TownAreaScript/TownAreaScript.inc"
}

// [b28-L14] P74/P26：ORIG C1ERKS_ 0x86d296a / aSERKS_ 0x86d2a1a 是编译器隐式合成
// （逐成员、跳过尾部对齐空洞），`= default` 外联定义既保持 W 符号发射又不再搬 pad。
#include "TownAreaScript/TownAreaScript_1.inc"
TownAreaScript& TownAreaScript::operator=(const TownAreaScript& other) = default;

#include "TownAreaScript/dtor_TownAreaScript.inc"
}

#include "TownAreaScript/clear.inc"
}
