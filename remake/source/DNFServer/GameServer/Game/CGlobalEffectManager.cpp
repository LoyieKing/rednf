// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CGlobalEffectManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CGlobalEffectManager / CGlobalEffect / TimerGlobalEffectItem*。
// 逐函数依据 docs/class_func_reports/（CGlobalEffectManager/*.md、CGlobalEffect/makeNotifyInfo.md、
// TimerGlobalEffectItemBegin|Timeout/registTimer.md）与 ORIG objdump 逐指令转写。
// 依赖符号（G_GameWorld/PacketGuard/CSystemTime/G_TimerQueue 等）均已由
// GameWorld.cpp / PacketGuard.cpp / CSystemTime.cpp / TimerQueue.cpp 提供。
#include "CGlobalEffectManager.h"

#include <math.h>
#include <string.h>

#include "GameWorld.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "TimerQueue.h"
#include "CSystemTime.h"
#include "GlobalData.h"

// 值编码常数：ORIG makeNotifyInfo 用 10000.0f 缩放（.rodata 0x8c854b0 = 0x461c4000 = 10000.0f）。
static const float kValueScale = 10000.0f;

// ============================================================================
// CGlobalEffect（0x2c）
// ============================================================================

// ORIG 0x084b7880：put_int(type) / put_int((int)(value*10000.0f) 截断) /
//               put_int(strlen(name)) / put_str(name,len) / put_int(endTime)。
#include "CGlobalEffectManager/makeNotifyInfo.inc"
}

// ORIG 0x084b800a：type@+0、value@+4、name@+8（无条件 strncpy 0x1e、无尾 0）、endTime@+0x28。
#include "CGlobalEffectManager/CGlobalEffect.inc"
}

// ORIG 0x082a7214：trivial dtor（仅 ret）。
#include "CGlobalEffectManager/dtor_CGlobalEffect.inc"
}

// ORIG 0x084b7fda：return m_type。
#include "CGlobalEffectManager/getType.inc"
}

// ORIG 0x084b7fe4：return m_value。
#include "CGlobalEffectManager/getValue.inc"
}

// ORIG 0x084b7ff8：return m_endTime <= curSec。
#include "CGlobalEffectManager/isTimeout.inc"
}

// ============================================================================
// CGlobalEffectManager（0x18）
// ============================================================================

// ORIG 0x082a721a：两个 vector<CGlobalEffect> 构造（成员自动）。
#include "CGlobalEffectManager/CGlobalEffectManager.inc"
}

// ORIG 0x08234fca：return m_activeEffects.size() + m_reservedEffects.size()。
#include "CGlobalEffectManager/getEffectNum.inc"
}

// ORIG 0x084b7d2c：遍历 m_activeEffects，任一 m_type==type 返回 true。
#include "CGlobalEffectManager/isAffectedEffect.inc"
}

// ORIG 0x084b7daa：遍历 m_activeEffects，累加所有 m_type==type 的 m_value。
#include "CGlobalEffectManager/getEffectValue.inc"
}

// ORIG 0x084b7ec4：put_header(0,0xb5)/put_byte(1)/put_int(size)，每项 makeNotifyInfo。
#include "CGlobalEffectManager/makeGlobalEffectInfo.inc"
}

// ORIG 0x084b7e48：PacketGuard，makeGlobalEffectInfo，finalize(true)，G_GameWorld()->send_all。
#include "CGlobalEffectManager/notifyGlobalEffectInfo.inc"
}

// ORIG 0x084b7932：若已受影响直接返回；按 beginSec 是否非零分派到活动/保留
// vector，并登记 TimerGlobalEffectItemBegin/Timeout。
#include "CGlobalEffectManager/registNewEffect.inc"
}

// ORIG 0x084b7c50：m_reservedEffects 中 m_type==type 者移到 m_activeEffects 并擦除。
#include "CGlobalEffectManager/beginReservedEffect.inc"
}

// ORIG 0x084b7a94：构造 0xb5/byte0 包，遍历 m_activeEffects 收集超时项（m_endTime<=curSec），
// 每项 makeNotifyInfo 并擦除，末尾回填计数并 send_all。
#include "CGlobalEffectManager/checkTimeout.inc"
}

// ============================================================================
// TimerGlobalEffectItem（ORIG 静态 helper，登记全局定时器）
// ============================================================================

// ORIG 0x08637e78：InsertTimer(OBJ_TYPE_2, 0, msg=0x77, beginSec, type, 0)。
#include "CGlobalEffectManager/registTimer.inc"
}
