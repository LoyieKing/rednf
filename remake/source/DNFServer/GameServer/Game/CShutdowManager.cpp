// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CShutdowManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CShutdowManager（G2-4 批次，关服管理人）。
// 定义 SendLastMsgDBQueue（ORIG _ZN15CShutdowManager18SendLastMsgDBQueueEP5CUser）
// 及默认构造（GetInstanceShutdowManager 的 static 单例需要）。
// （注：上游单例 GetInstanceShutdowManager 由 CGameManager.cpp TU 提供。）

#include "CShutdowManager.h"
#include "ThreadInterface.h"

#include <cstdio>

#include "CUser.h"

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ORIG W 0x082a6fa8：ctor 即 Reset()（清零 +0x00/+0x04/+0x08）。
#include "CShutdowManager/CShutdowManager.inc"
}

// ORIG 0x82a37c8：this 使能位(+0x00) 非零时向 DB 队列投递两条关服消息：
//   queue 2: << 0x95 << -1；queue 4: << 0x96 << -1；各使 this+0x04 计数 +1。
//   （ORIG 全程未使用 user 参数。）
// [b28d-27] DIFF N82：ORIG 0x082a37c8；p167 CLEAR（20/20），残差为帧/槽与
// 分支块排布，语义闭合。
#include "CShutdowManager/SendLastMsgDBQueue.inc"
}

#include "CShutdowManager/WaitUntilExitThread.inc"
}

CShutdowManager* GetInstanceShutdowManager()
{
    static CShutdowManager s;
    return &s;
}
// ORIG 0x082a6f88（_ZN15CShutdowManager5ResetEv，W）：
//   this+0 = 0；this+4 = 0；this+8 = 0。
#include "CShutdowManager/Reset.inc"
}

// ORIG 0x082a6f70（_ZN15CShutdowManager14EnableShutdownEb，W）：
//   *(bool*)this = on。
#include "CShutdowManager/EnableShutdown.inc"
}

// ORIG 0x082a36bc（_ZN15CShutdowManager15OnAllUserLogoutEv，T，70 条）：
//   !m_flag → return；cMyTrace(PRETTY,0x323b,0)("Send message to monitor
//   for logout all user.")；guard 单例 stream：StreamPool::Acquire(
//   GlobalData::s_stream_pool, 0x323d) → CStreamGuard(s,true) << 0x3f
//   << -1；GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard)；
//   GlobalData::s_server_stop = 1。
#include "CShutdowManager/OnAllUserLogout.inc"
}

// ORIG 0x084ed1ea（_ZN15CShutdowManager29IncrementLastMsgResponseCountEv，W）：
//   ++*(int*)(this+0x08)。
#include "CShutdowManager/IncrementLastMsgResponseCount.inc"
}

// ORIG 0x084ed1fe（_ZN15CShutdowManager15GetLastMsgCountEv，W）：
//   return *(unsigned int*)(this+0x04)。
#include "CShutdowManager/GetLastMsgCount.inc"
}

// ORIG 0x084ed20a（_ZN15CShutdowManager23GetLastMsgResponseCountEv，W）：
//   return *(unsigned int*)(this+0x08)。
#include "CShutdowManager/GetLastMsgResponseCount.inc"
}
