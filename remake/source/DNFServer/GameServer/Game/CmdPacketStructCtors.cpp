// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CmdPacketStructCtors/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r CMDPacketStruct 生命周期符号实现（OnlyOrig_Exchange 批）
//
// worklist /tmp/only_orig/CMDPacketStruct.txt 全部 77 个符号由本 TU + 头文件
// 内联基类产生：
//   - C1/C2：本 TU 显式 out-of-line 定义（一对 C1/C2 由同一函数体定义生成）。
//   - D0/D1/D2：STBaseResponse 虚析构链使派生 Res 类自动生成弱析构
//     （ORIG 同为 W；D0=D1+operator delete，D1=复位 vptr+ParamBase::D1，
//     与 ORIG 逐字节形态一致，见 /tmp/es_disasm.txt）。
// memset 常量为 ORIG ctor 反汇编字面量（证据见 CMDPacketStruct.h 注释）。
// ============================================================================

#include "CMDPacketStruct.h"

namespace CMDPacketStruct
{

// ---- Pause ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarPause.inc"
}
#include "CmdPacketStructCtors/_ResAdvanceAltarPause.inc"
}

// ---- BuyItem ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarBuyItem.inc"
}
#include "CmdPacketStructCtors/_ResAdvanceAltarBuyItem.inc"
}

// ---- SetSlot ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarSetSlot.inc"
}
#include "CmdPacketStructCtors/_ResAdvanceAltarSetSlot.inc"
}

// ---- ResetStar ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarResetStar.inc"
}
#include "CmdPacketStructCtors/_ResAdvanceAltarResetStar.inc"
}

// ---- SummonUnit ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarSummonUnit.inc"
}
#include "CmdPacketStructCtors/_ResAdvanceAltarSummonUnit.inc"
}

// ---- UpgradeGage ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarUpgradeGage.inc"
}
#include "CmdPacketStructCtors/_ResAdvanceAltarUpgradeGage.inc"
}

// ---- ExchangeSlot ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarExchangeSlot.inc"
}
#include "CmdPacketStructCtors/_ResAdvanceAltarExchangeSlot.inc"
}

// ---- GetAchievementReward ----
#include "CmdPacketStructCtors/_ReqAdvanceAltarGetAchievementReward.inc"
}
#include "CmdPacketStructCtors/_ResAdvanceAltarGetAchievementReward.inc"
}

// ---- EventDungeon ClearRoom / DestoryObject ----
#include "CmdPacketStructCtors/_STReqEventDungeonClearRoom.inc"
}
#include "CmdPacketStructCtors/_STResEventDungeonClearRoom.inc"
}
#include "CmdPacketStructCtors/_STReqEventDungeonDestoryObject.inc"
}
#include "CmdPacketStructCtors/_STResEventDungeonDestoryObject.inc"
}

}  // namespace CMDPacketStruct
