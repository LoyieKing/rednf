// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPowerWarEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 力战事件 CPowerWarEvent 实现。
// 报告：docs/class_func_reports/CPowerWarEvent/ 全部 7 个方法
// （CPowerWarEvent / ~CPowerWarEvent / ~CPowerWarEvent_082679dc /
//  StartEvent / StartEvent_08267dea / EndEvent / CleanupPowerWarServerMessage）。
#include "CPowerWarEvent.h"

#include "CEnvironment.h"          // G_CEnvironment / m_serverEnvir.m_serverType / m_channelNo
#include "CDataManager.h"          // G_CDataManager / m_serverParameter / m_powerParameter
#include "TimerDispatcher.h"       // Timer_DungeonInoutOpenTime::checkDungeonInoutTime
#include "GameWorld.h"             // G_GameWorld / send_all / OnEndPowerWar / CancelPowerUp
#include "TimerQueue.h"            // G_TimerQueue / TimerEntry / TIMER_MESSAGE
#include "CPowerManager.h"         // GlobalData::s_power_manager 方法
#include "CMonitorServerProxy.h"   // CMonitorServerProxy::SendTcpPacket
#include "CServerProxyMgr.h"       // CServerProxyMgr::GetServerProxy
#include "Packet_Monitor_Event_End.h"
#include "PacketGuard.h"           // PacketGuard / put_header / put_byte / put_int / put_str / finalize
#include "CGameManager.h"          // G_CGameManager / GetSharedServerMessageManager
#include "CSharedServerMessageManager.h"  // CSharedServerMessageManager::DeleteSpecificMessage
#include "CAssaultMgr.h"           // pvp_assault::GetInstanceAssaultMgr
#include "GameTypes.h"             // cMyTrace
#include <cstring>                 // strlen

// ---------------------------------------------------------------------------
// ORIG 0x08267990：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CPowerWarEvent/CPowerWarEvent.inc"
}

// ORIG 0x082679ac / 0x082679dc：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CPowerWarEvent/dtor_CPowerWarEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08267dea：环境为 1/2 时向 monitor 发 Event_End 包；
// 否则事件未进行中则直接返回，进行中则开始力战（关终极副本、发配置、
// 挂三个定时器）。
// [b28d-solo-CPowerWarEvent] AE（446h47 路由后）。P221 全表达式临时链收口槽位/块序；
//   唯一曾存残差=虚调 IsEventing(0) 间接目标寄存器（ORIG 0x08267e80 `mov (%eax),%edx` +
//   0x08267e90 `call *%edx`、this 入 %eax；4.4.7 恒目标 %eax、this 入 %edx = N7）。
//   P230：本件属 N7 敏感族，Main 已将本 TU 纳入 446h47「仅换 4.4.6 后端」路由 ⇒
//   compare_tu_game_orig: identical=2 ae=11 near=0（本件由 NEAR 转 AE）；p167 CLEAR 50=50。
#include "CPowerWarEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x082679fa：同 StartEvent(Word_Param)，但不写 m_param。
// [b28d-solo-CPowerWarEvent] AE（446h47 路由后）。与 StartEvent(Word_Param) 同族：
//   P221 临时链后唯一曾存残差=虚调 IsEventing(0) 目标寄存器（ORIG 0x08267a90
//   `mov (%eax),%edx` + 0x08267aa0 `call *%edx`，this 入 %eax；4.4.7 恒 %eax/%edx 对调）。
//   P230：446h47 路由后本件由 NEAR 转 AE（compare_tu_game_orig near 3→0）；p167 CLEAR 50=50。
#include "CPowerWarEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x082681e2：环境为 1/2 时直接返回；事件未进行中直接返回；
// 进行中则关力战（开终极副本、OnEndPowerWar/CancelPowerUp 广播、
// 清共享服务器消息、强制关闭 assault、触发 0x35 事件）。
// [b28d-solo-CPowerWarEvent] AE（446h47 路由后）。P221 临时链（含 0xde/0xe1/0xe4/0x110
//   四 trace）收口槽位/块序，唯一曾存残差=虚调 IsEventing(0) 目标寄存器（ORIG 0x08268226
//   `mov (%eax),%edx` + 0x08268236 `call *%edx`、this 入 %eax；4.4.7 恒目标 %eax = N7）。
//   P230：446h47 路由后本件由 NEAR 转 AE（compare_tu_game_orig near 3→0）；p167 CLEAR 34=34。
//   另修正真语义 bug：0xe4 trace 文本 ORIG .rodata 0x8bf0544 为 "…DUNGEON OPEN!!"（旧写 CLOSE）。
#include "CPowerWarEvent/EndEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08268492：删共享消息 1/2/3 后广播三条空串公告（send_all_with_state 3）。
#include "CPowerWarEvent/CleanupPowerWarServerMessage.inc"
}
