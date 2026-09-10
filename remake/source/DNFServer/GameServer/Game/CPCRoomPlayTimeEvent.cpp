// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPCRoomPlayTimeEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CPCRoomPlayTimeEvent 实现（网吧时长活动事件）。
// 依据 ORIG elf 反汇编（报告目录缺失，nm 定址 0x0811c540 后 objdump 转写）：
//   C1 0x0811c55a：CEventBase() + vptr
//   D1 0x0811c576 / D0 0x0811c5a6：vptr 恢复 + ~CEventBase
//   StartEvent 0x0811c5c4：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__, 0x15, 0)
//   StartEvent(Word_Param) 0x0811c616：虚转发 StartEvent()（vtable+0x8 槽）
//   EndEvent 0x0811c630：SetEventFlag(false) + 日志 + 遍历 m_UserInWorld 清网吧时长数据
//   StartAction 0x0811c754：遍历 m_UserInWorld 置网吧起始时间并通知
#include "CPCRoomPlayTimeEvent.h"

#include <map>

#include "CUser.h"          // isAffectedPremium/get_state/setPcRoomPlayTimeData
#include "GameWorld.h"      // GameWorld::m_UserInWorld (+0x134)
#include "GlobalData.h"     // s_systemTime_ (0x941f714)
#include "CSystemTime.h"    // getCurSec

#include "CPCRoomPlayTimeEvent/CPCRoomPlayTimeEvent.inc"
}

#include "CPCRoomPlayTimeEvent/dtor_CPCRoomPlayTimeEvent.inc"
}

#include "CPCRoomPlayTimeEvent/StartEvent.inc"
}

#include "CPCRoomPlayTimeEvent/StartEvent_1.inc"
}

#include "CPCRoomPlayTimeEvent/EndEvent.inc"
}

#include "CPCRoomPlayTimeEvent/StartAction.inc"
}
