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
NOTIPacketStruct::_NotiAdvanceAltarSpawnUnit::_NotiAdvanceAltarSpawnUnit()
{
    std::memset(this, 0, 0x28);
}

// ---- StageInfo ----
NOTIPacketStruct::_NotiAdvanceAltarStageInfo::_NotiAdvanceAltarStageInfo()
{
    std::memset(this, 0, 8);
}

// ---- MapInfo ----
NOTIPacketStruct::_NotiAdvanceAltarMapInfo::_NotiAdvanceAltarMapInfo()
{
    std::memset(this, 0, 0x14);
}

// ---- ClearStage ----
// ORIG 0x08134720（0x31）：_Stage ctor(this+0xd) → memset(this,0,0x1c)
NOTIPacketStruct::_NotiAdvanceAltarClearStage::_NotiAdvanceAltarClearStage()
{
    std::memset(this, 0, 0x1c);
}

// ---- EplpState ----
NOTIPacketStruct::_NotiAdvanceAltarEplpState::_NotiAdvanceAltarEplpState()
{
    std::memset(this, 0, 8);
}

// ---- UpdateRidable ----
NOTIPacketStruct::_NotiAdvanceAltarUpdateRidable::_NotiAdvanceAltarUpdateRidable()
{
    std::memset(this, 0, 1);
}
