// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W9_EventSmall/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// W9_EventSmall.cpp —— Wave9 事件小类批（5 类）真实实现。
// 声明见 W9_EventSmall.h。逐函数依据 docs/class_func_reports/ 对应报告 +
// ORIG objdump（df_game_r）转写；无独立报告的小函数（Inc*/Get*/D0 等）
// 由 objdump 直接核对，地址注释在各函数头。
//
//   CTrcOnOffEvent                    报告目录 CTrcOnOffEvent/
//   CFatigueBuffEvent                 无报告，ORIG 0x0811bbxx.. 反汇编实证
//   CWeekendBonusEvent                无报告，ORIG 0x0811755x.. 反汇编实证
//   CAutoMarketConditionsControlEvent 报告目录 CAutoMarketConditionsControlEvent/
//   CCharacBloodDungeon               报告目录 CCharacBloodDungeon/ + 0x85bfexx
// ============================================================================

#include <ctime>
#include "W9_EventSmall.h"
#include "GameTypes.h"       // cMyTrace
#include "GlobalData.h"
#include "CSystemTime.h"
#include "StreamPool.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"
#include "CUser.h"

// getDailyInitializeTime：定义 GameUtils.cpp（DB_LoadEtc.cpp 已声明）。
#include "W9_EventSmall/getDailyInitializeTime.inc"
#include "W9_EventSmall/CTrcOnOffEvent.inc"
#include "W9_EventSmall/makeRequest.inc"
}

// ============================ 事件基类模板 ============================
// 四个小类的 Start/End 全部为「SetEventFlag + cMyTrace(function 名, len, 0) +
// trace(msg)」同构（各报告反编译一致）。构造/析构只置 vptr（编译器生成），
// IsEventing 为基类透传。

#include "W9_EventSmall/dtor_CTrcOnOffEvent.inc"

#include "W9_EventSmall/StartEvent.inc"
}

#include "W9_EventSmall/EndEvent.inc"
}

#include "W9_EventSmall/StartEvent_1.inc"
}

#include "W9_EventSmall/IsEventing.inc"
}

// ---- CFatigueBuffEvent ----
#include "W9_EventSmall/CFatigueBuffEvent.inc"
#include "W9_EventSmall/dtor_CFatigueBuffEvent.inc"

#include "W9_EventSmall/StartEvent_2.inc"
}

#include "W9_EventSmall/EndEvent_1.inc"
}

#include "W9_EventSmall/StartEvent_3.inc"
}

// ---- CWeekendBonusEvent ----
#include "W9_EventSmall/CWeekendBonusEvent.inc"
#include "W9_EventSmall/dtor_CWeekendBonusEvent.inc"

#include "W9_EventSmall/StartEvent_4.inc"
}

#include "W9_EventSmall/EndEvent_2.inc"
}

#include "W9_EventSmall/StartEvent_5.inc"
}

// ---- CAutoMarketConditionsControlEvent ----
#include "W9_EventSmall/CAutoMarketConditionsControlEvent.inc"
#include "W9_EventSmall/dtor_CAutoMarketConditionsControlEvent.inc"

#include "W9_EventSmall/StartEvent_6.inc"
}

#include "W9_EventSmall/EndEvent_3.inc"
}

#include "W9_EventSmall/StartEvent_7.inc"
}

// ============================ CCharacBloodDungeon ============================
#include "W9_EventSmall/CCharacBloodDungeon.inc"
}

#include "W9_EventSmall/_reset.inc"
}

#include "W9_EventSmall/_saveData.inc"
}

#include "W9_EventSmall/loadData.inc"
}

#include "W9_EventSmall/getData.inc"
}

#include "W9_EventSmall/ResetDaily.inc"
}

// [b28d-lane6] AE（已修）：字段 m_point unsigned 化（P211）复现 ORIG 0x85bfee6 add 段。
#include "W9_EventSmall/IncPoint.inc"
}

#include "W9_EventSmall/IncUseGold.inc"
}

#include "W9_EventSmall/IncEnterCount.inc"
}

#include "W9_EventSmall/GetEnterCount.inc"
}

#include "W9_EventSmall/GetWeeklyEnterCount.inc"
}
