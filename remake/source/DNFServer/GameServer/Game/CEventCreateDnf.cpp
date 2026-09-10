// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CEventCreateDnf/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "CEventCreateDnf.h"
#include "GlobalData.h"     // s_systemTime_（LinkE2）
#include "TimerQueue.h"     // G_TimerQueue/InsertTimer/TIMER_MESSAGE（LinkE2）
#include "GameTypes.h"      // cMyTrace（LinkE2）
#include "CreateDnfEventSciprt.h"   // CreateDnfEventSciprtServer::GetItemInfo（StubS3A SendReward）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_.findString
#include "CMailBoxHelper.h"           // WongWork::CMailBoxHelper 静态邮件接口
#include "CUser.h"                    // CUser::getCurCharacNo/GetServerGroup/Send（完整类型）
#include "PacketGuard.h"
#include "DBSelectCreateDnfEventInfo.h"   // makeRequest（RegistStartTimer ORIG 0x08163fc2）
#include "LogManager.h"                   // logFormat（StartEvent(Word_Param) ORIG 0x08163cb6）


#include <ctime>

#include "CEventCreateDnf/CEventCreateDnf.inc"
}

#include "CEventCreateDnf/dtor_CEventCreateDnf.inc"

#include "CEventCreateDnf/setState.inc"
}

#include "CEventCreateDnf/GetRate.inc"
}

#include "CEventCreateDnf/IsRewardCondition.inc"
}

#include "CEventCreateDnf/SetEventInfo.inc"
}

#include "CEventCreateDnf/StartEvent.inc"
}

#include "CEventCreateDnf/StartEvent_1.inc"
}

#include "CEventCreateDnf/EndEvent.inc"
}

#include "CEventCreateDnf/RegistStartTimer.inc"
}

#include "CEventCreateDnf/RegistNextTimer.inc"
}

#include "CEventCreateDnf/SendReward.inc"
}
