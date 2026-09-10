// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 InterSelectCreateDnfEventInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— InterSelectCreateDnfEventInfo（台湾内部事件分发器）。
// 依据报告：
//   makeRequest  docs/class_func_reports/InterSelectCreateDnfEventInfo/makeRequest.md（0x8160ef6）
//   dispatch_sig docs/class_func_reports/InterSelectCreateDnfEventInfo/dispatch_sig.md（0x8160d5e）
//   Reward       docs/class_func_reports/InterSelectCreateDnfEventInfo__Reward/（0x8160c4a / 0x8160c6e）
//   C1/D1/D0     objdump 0x816201a/0x816203e/0x816206e（无报告，nm 定址转写）
// 依赖：TaiwanInterfaceInterDispatcher（基类）实现由 OrigTaiwanDisp 落；
// CEventCreateDnf / CEventManager / MsgQueueMgr / StreamPool 已有权威实现。
#include "InterSelectCreateDnfEventInfo.h"

#include "CInventory.h"      // PacketGuard 完整定义（含 InterfacePacketBuf 基础方法）
#include "CUser.h"
#include "CStreamGuard.h"
#include "TaiwanInternal.h"  // Taiwan::internal_stream
#include "GlobalData.h"      // s_stream_pool / s_msgq_mgr / s_event_manager
#include "MsgQueueMgr.h"
#include "CGuildServerProxy.h"  // CEventManager（GetRepeatEvent）
#include "CEventCreateDnf.h"
#include "GameWorld.h"       // ProcessAllUsers 模板 + G_GameWorld()
#include "LogManager.h"

// ---- Reward ----
// ORIG 0x8160c4a：+0x00 short（rate），+0x02 char（flag）。
#include "InterSelectCreateDnfEventInfo/Reward.inc"
}

// ORIG 0x8160c6e：组包 0x22d，rate + 奖励条件字节，finalize 后发送。
// Ghidra C：put_header(0,0x22d); put_short(m_rate);
//   if (GetEventCreateDnfReward() == 1 || m_flag == 0) put_byte(0) else put_byte(1);
//   finalize(true); user->Send(guard);
#include "InterSelectCreateDnfEventInfo/operator.inc"
}

// ---- C1 / D1 / D0 ----
// ORIG 0x816201a：call 基类 ctor(code=9) + 设 vtable 0x8b7aca8。
#include "InterSelectCreateDnfEventInfo/InterSelectCreateDnfEventInfo.inc"
}

// ORIG 0x816203e：设 vptr + 调基类 D1 + 尾部 delete 检查（D1 无 delete）。
#include "InterSelectCreateDnfEventInfo/dtor_InterSelectCreateDnfEventInfo.inc"
}

// ORIG 0x816206e：call D1 + operator delete。
// （编译器生成 D0，无需手写。）

// ---- makeRequest ----
// ORIG 0x8160ef6：Acquire stream → CStreamGuard(stream,true) →
// internal_stream(guard, 9, 0xffffffff) → guard->GetInBuffer<Msg>()；
// 写 hour/rate/eventDay 三 int 入 MsgQueueMgr::put(1, guard)。
#include "InterSelectCreateDnfEventInfo/makeRequest.inc"
}

// ---- dispatch_sig ----
// ORIG 0x8160d5e。第二参 data 实际承载 Msg*（ORIG 汇编 0x10(%ebp)=data 判空；
// Ghidra C 的 param_3 标注为反编译器 thiscall 参数索引偏差，以汇编为准）。
// 流程：
//   data==0 → 日志 "interMsg is null."，返回 0xfe；
//   event = GetRepeatEvent(0xa0)；
//   if (event==0 || vtable+0x34 槽(event,0) != 1) 视为「事件未进行」：
//       再调槽取字节 + 日志 stayEvent=ok/nullptr eventing:%d，返回 0x108；
//   否则：hour<0 时 hour=GetRate()&0xffff；SetEventInfo(rate,hour,eventDay)；
//       Reward(GetRate()&0xffff, IsRewardCondition()) 经 ProcessAllUsers 分发，返回 0。
// 注：vtable+0x34 槽为 CEventBase 判活直调（与 Dispatchers.cpp BingoQuiz
// check_error 同口径；CGuildServerProxy.h 槽位注释与 ORIG 槽序有出入，用直调保 ABI）。
#include "InterSelectCreateDnfEventInfo/dispatch_sig.inc"
}

// CEventCreateDnf::GetState 定义置于本 TU（与 CEventCreateDnf.cpp 中
// IsRewardCondition 调用者分离）：4.4 unit-at-a-time 对 TU 内可见 callee
// 塌缩 outgoing 帧（sub $0x4），ORIG 4.1 恒发 sub $0x18（探针 p6/p7 同族）。
#include "InterSelectCreateDnfEventInfo/GetState.inc"
}
