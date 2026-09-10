// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 NOTIPacketStructCtors/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r NOTIPacketStruct 生命周期符号实现（OrigNOTIPkt 批）
//
// ORIG 12 个符号全部由本 TU 的 6 个 out-of-line ctor 定义生成
// （GCC 对每个 ctor 定义发射 C1/C2 一对符号）：
//   _ZN16NOTIPacketStruct24_NotiAdvanceAltarMapInfoC1/C2Ev      @0x081346fc (0x23)
//   _ZN16NOTIPacketStruct26_NotiAdvanceAltarEplpStateC1/C2Ev    @0x08134752 (0x23)
//   _ZN16NOTIPacketStruct26_NotiAdvanceAltarSpawnUnitC1/C2Ev    @0x081346a6 (0x31)
//   _ZN16NOTIPacketStruct26_NotiAdvanceAltarStageInfoC1/C2Ev    @0x081346d8 (0x23)
//   _ZN16NOTIPacketStruct27_NotiAdvanceAltarClearStageC1/C2Ev   @0x08134720 (0x31)
//   _ZN16NOTIPacketStruct30_NotiAdvanceAltarUpdateRidableC1/C2Ev @0x08141ff8 (0x23)
// memset 常量为 ORIG ctor 反汇编字面量（字段注释见 NOTIPacketStruct.h）。
// SpawnUnit/ClearStage 的子对象 ctor（_SummonObject @ this+0x4 /
// _Stage @ this+0xd）由编译器在 ctor 体前自动调用（头内真实类型成员），
// 与 ORIG「先子对象构造、再整体 memset」一致。
// ============================================================================

#include "NOTIPacketStruct.h"

#include <cstring>

// ---- SpawnUnit ----
// ORIG 0x081346a6（0x31）：_SummonObject ctor(this+0x4) → memset(this,0,0x28)
#include "NOTIPacketStructCtors/_NotiAdvanceAltarSpawnUnit.inc"
}

// ---- StageInfo ----
#include "NOTIPacketStructCtors/_NotiAdvanceAltarStageInfo.inc"
}

// ---- MapInfo ----
#include "NOTIPacketStructCtors/_NotiAdvanceAltarMapInfo.inc"
}

// ---- ClearStage ----
// ORIG 0x08134720（0x31）：_Stage ctor(this+0xd) → memset(this,0,0x1c)
#include "NOTIPacketStructCtors/_NotiAdvanceAltarClearStage.inc"
}

// ---- EplpState ----
#include "NOTIPacketStructCtors/_NotiAdvanceAltarEplpState.inc"
}

// ---- UpdateRidable ----
#include "NOTIPacketStructCtors/_NotiAdvanceAltarUpdateRidable.inc"
}
