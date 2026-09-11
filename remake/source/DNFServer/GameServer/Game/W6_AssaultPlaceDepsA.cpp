// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W6_AssaultPlaceDepsA/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ pvp_assault::CAssaultPlace 前半 30 个 ORIG-only 方法（Wave6）。
// 组：W6_AssaultPlaceA（符号清单 /tmp/only_orig/pvp_assault.txt 第 24-53 行）。
// 全部语义取自 docs/class_func_reports/pvp_assault__CAssaultPlace/ 对应报告
// （汇编 + Ghidra C）；每函数前注释 ORIG 地址/尺寸。
// [b28d-solo-W6_AssaultPlaceDepsA 收口] 清单 9 件：_AvailableAssault/StartAssault/
//   TimeoutConnectP2P 三件修至 AE（负极性包裹 P59 / P227 name 内联升格 /
//   P0 删 log 具名槽 + P223 双标签守卫岛 + 语句表达式 setne）；其余 6 件封 N
//   （EndAssault N82附、RevivalUser N104、_ProcessChaos N7/P75/N121、
//   _SendHoldVictory N98/N82、EvadeAssaultPlace N18b/N7/N108、
//   TimeoutEndAssault N18a），逐条实测数据见各函数头判词。
// [G5 2026-09-10 · L12 round2 组5 复核] 6 件全核：p167 逐件核对，EvadeAssaultPlace CLEAN，
//   其余 MISSING=EXTRA 同数（C1/C2、D1/D2 同址别名）⇒ **无语义缺口**。
//   · _SendHoldVictory（NEAR）：ORIG s=-0x3c / member=-0x38 / name=-0x34 ↔ 我方
//     s=-0x38 / member=-0x34 / name=-0x30 ⇒ **整体平移 4**（N126/N95 族，非相对序差，
//     判据⑪ 判定后不再做声明重排）。
//   · _ProcessChaos / EvadeAssaultPlace：ORIG `call; mov 0x8(%ebp),%edx;
//     mov 0x124(%edx),%edx; cmp %edx,%eax` ↔ 我方 eax↔edx 角色互换（N7/N22；交换操作数
//     在 CHCT/TimeoutEndAssault 已实测反劣）。
//   · TimeoutEndAssault：残余 1 条 regmove（`mov 0x10c(%eax),%eax; mov %eax,%ebx`）；
//     `(ENUM_ASSAULT_STATE)(unsigned int)` 替代形仍单段（P265 在本后端不成立）⇒ N18a。
//   · EndAssault（NEAR）：槽集合两侧全等、相对序差（P309 头部声明试验已证无效）。
//   · RevivalUser：仅 _ZTI/全局 reloc 显示差 + 1 条，AE 后等价。
// [L2-S5 2026-09-11 复核] 5 件逐个复测 + 2 个新旋钮实验：
//   · **_SendHoldVictory：NEAR → AE（翻正，201=201）**。手法 = P212「**全**头部声明 +
//     声明序按 ORIG 槽深→浅序」——旧判词 N98「声明序/头部声明不可翻」被此组合推翻
//     （详见该函数头判词：14 个标量全部提前 + 按 0x082ecb59..0x082ecd30 实录槽址排序）。
//   · EndAssault：新增实验-D（aliveCounts 提到 i/allDead 之前、声明序深→浅）⇒ 槽图
//     逐条不变（packet 恒占 -0x24），再证 N82附「块内聚合优先低槽」固定分区，已回滚。
//   · RevivalUser：复核 N104（unwind 块位置：ORIG 在函数末、4.4 就地 fall-through）——
//     非槽问题（两侧槽集全等），P212 不适用。
//   · _ProcessChaos：复核 N7/P75/N121（5 组装载序/寄存器角色 + 尾部双 jmp），P212 不适用。
//   · EvadeAssaultPlace：基线即 AE，未触碰。
//   · GI 假缺口：`_Z41__static_initialization_and_destruction_0ii` —— ORIG 该符号有
//     493 个实例，工具按名错配取首个（0x0807ee76 属 PacketTypes）⇒ 逐 TU 读数不可比，
//     我方 16 条 ioinit 形正确（Main 2026-09-11 更正判据）⇒ **零改动**。
#include "CAssaultMgr.h"

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "PacketBuf.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"           // s_event_manager / s_power_manager / s_systemTime_
#include "TimerDispatcher.h"      // TimerCloseAssault/TimerEndAssault/TimerConnectP2PAssaultTimeout
#include "CPowerManager.h"        // IsPowerWarEventOn / IsAbleToRevival / GetPowerWarLog(CPowerWarLog)
#include "CGuildServerProxy.h"    // CEventBase::IsEventing（vtable+0x34 槽）
#include "GameWorld.h"            // G_GameWorld / send_all / send_to_vill / get_disposition
#include "CDisconnectDetecter.h"  // disconnect_detecter::RegisterUser
#include "CDataManager.h"         // PowerParameterScript::GetPenalty/GetBreakBonus (G_CDataManager()+0xa628)
#include "Village.h"

namespace pvp_assault
{

#include "W6_AssaultPlaceDepsA/_GetTeamCount.inc"


#include "W6_AssaultPlaceDepsA/_GetTeamCount_1.inc"


#include "W6_AssaultPlaceDepsA/_MakeChangeHp.inc"


#include "W6_AssaultPlaceDepsA/_MakeGiveAlarm.inc"


#include "W6_AssaultPlaceDepsA/_MakeEndAssault.inc"


#include "W6_AssaultPlaceDepsA/_IsProcsssChaos.inc"


#include "W6_AssaultPlaceDepsA/_IsChangeDieState.inc"


#include "W6_AssaultPlaceDepsA/_SetReadyState.inc"


#include "W6_AssaultPlaceDepsA/DoConnectP2P.inc"


#include "W6_AssaultPlaceDepsA/_BackupPartyInfo.inc"


#include "W6_AssaultPlaceDepsA/_SendGiveAlarm.inc"


#include "W6_AssaultPlaceDepsA/_SendUserInfo.inc"


#include "W6_AssaultPlaceDepsA/_CheckAssaultable.inc"


#include "W6_AssaultPlaceDepsA/ClearBadP2PUser.inc"


#include "W6_AssaultPlaceDepsA/_ChangeDieState.inc"


#include "W6_AssaultPlaceDepsA/_ProcessChaos.inc"


#include "W6_AssaultPlaceDepsA/StartAssault.inc"


#include "W6_AssaultPlaceDepsA/EndAssault.inc"


#include "W6_AssaultPlaceDepsA/TimeoutEndAssault.inc"


// ---------------------------------------------------------------------------
// ORIG T 0x82e9502 系列使用的 enemy 局部逻辑已并入上面 StartAssault；
// 本段以下为 ConnectP2P/CompleteLoad/Revival/Evade/Timeout 等。
// ---------------------------------------------------------------------------

#include "W6_AssaultPlaceDepsA/ConnectP2P.inc"


#include "W6_AssaultPlaceDepsA/CompleteLoad.inc"


#include "W6_AssaultPlaceDepsA/TimeoutConnectP2P.inc"


#include "W6_AssaultPlaceDepsA/RevivalUser.inc"


#include "W6_AssaultPlaceDepsA/EvadeAssaultPlace.inc"


#include "W6_AssaultPlaceDepsA/_SendHoldVictory.inc"


#include "W6_AssaultPlaceDepsA/_VictoryBonus.inc"


#include "W6_AssaultPlaceDepsA/_SeparateTeam.inc"


#include "W6_AssaultPlaceDepsA/_DispositionUser.inc"


#include "W6_AssaultPlaceDepsA/_MakeUserInfo.inc"


#include "W6_AssaultPlaceDepsA/_AvailableAssault.inc"


#include "W6_AssaultPlaceDepsA/CAssaultPlace.inc"

}  // namespace pvp_assault
