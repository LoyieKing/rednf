// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMissionList_Charac/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 角色任务列表实现（G3-2 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/CMissionList_Charac.md（Ghidra C）+ ORIG
// 反汇编（ctor 0x832af8a / dtor 0x85ec626 / 弱方法 0x85e68b6..0x85e7130）。
// [b28d-64 判词汇总 · 泳道64] 本 TU 10 件（明细 /tmp/b28d_lane_64_result.tsv）：
//   addNewMission 54/54 AE；Update_Revenge_event 97/97 NEAR·N82（槽号）；
//   Update_EnterWorld_event 85/83 N封顶（P133 短路 bool 物化跨版本）；
//   Update_RecvPacket_event 116/117 N82；ClearMission 123/121 N7/N82；
//   MakeMissionList_JustKind 96/104 N82；MakeMissionList_forNewbies 93/105 N82；
//   check_mission_kind 109/114 N7/N82；MakeMissionList_forOldUser 109/121 N82；
//   loadData 156/180 N82；Update_ComboMission_Event 150/135 N82。
// [b28d-solo-CMissionList_Charac 判词汇总 · 单文件泳道] 清单 7 件最终结果
// （明细 /tmp/b28d_solo_CMissionList_Charac_result.tsv）：
//   Update_Revenge_event 97/97 AE（param.m_victory 成员写入，修死局部误读）；
//   Update_EnterWorld_event 85/85 AE（param.m_channelTo/m_channelFrom 成员写入，
//   修死局部误读——两条 store 目标为 pack(1) 成员 @+0x0b/+0x07）；
//   BitsetToStr(Ss) 51/51 AE、BitsetToStr(Ph) 44/44 AE（累加器 volatile 化复现
//   4.1 读-加-写三段 + 槽位；Ph 另删 `int j=0` 前置多出的 j 初始化）；
//   check_mission_kind 109/109 **AE**（[F3b-C05③ 2026-09-10] 旧判 N121 作废：
//   平铺 `if (m_index == 0) return;` 在 4.4 产一组 nop+jmp 中继岛；改正向包裹
//   `if (m_index != 0) { … }` 后 fail 路径 jump-thread 直达共享尾出口）；
//   Reset_DailyMission 132/133 N104、Update_ComboMission_Event
//   150/149 N104（EH 正常尾块/unwind 块物理序互换 + 少 1 岛，指令集全等）。
//   P0：本文件无 register asm 绑定；p167 七件全 CLEAR；TU COMPILE OK 0 错误。
#include <bitset>
#include <cstring>
#include <map>
#include <utility>
#include <vector>

#include "CMissionList_Charac.h"
#include "CMission.h"
#include "CUser.h"   // 经 SigTypes.h 提供权威 SIG_REQUEST_PVP_MISSION_LIST（0x14a）
#include "CDataManager.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"
#include "PacketGuard.h"

// PvP_Room 最小声明（PvP_Room.h 引入 PvP_deps.h 的重复 PVP_BATTLE_MODE，
// 本 TU 只需 get_pvp_battle_mode）
class PvP_Room
{
public:
    int get_pvp_battle_mode();
};

// ============================================================================
// 本 TU 需要而其它 TU 提供的符号（ORIG 真实符号，形态按 mangled 名转录）
// ============================================================================
// CUser::GetPVPRoom / cUserHistoryLog::pvpMissionAdd/pvpMissionDel 均已在
// CUser.h 权威声明，直接真实调用（原 extern "C" asm 桥已删除）。


#include "CMissionList_Charac/StrToBitset.inc"

#include "CMissionList_Charac/BitsetToStr.inc"


#include "CMissionList_Charac/BitsetToStr_1.inc"

#include "CMissionList_Charac/CheckDailyScheduleTime.inc"
                            long lCurTime);

// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；与 CInventory.h
// 的占位版本不同，本 TU 使用真实形态）


// [b28d-solo-CMissionList_Charac] P154 发明符号清理：原 `static int
// GetDailyScheduleTime(CEnvironment*)` 在 ORIG nm 中**不存在**（仅 T
// CheckDailyScheduleTime），ORIG loadData 0x085e49d0 直接内联
// `mov 0x37c(%eax),%eax`（= m_serverEnvir.m_fatigueTime，CEnvironment.h 权威布局）
// ⇒ 删除该 helper，调用点改为具名字段内联。

// ============================================================================
// 构造 / 析构 / 虚接口
// ============================================================================

#include "CMissionList_Charac/CMissionList_Charac.inc"


#include "CMissionList_Charac/dtor_CMissionList_Charac.inc"


#include "CMissionList_Charac/loadData.inc"


#include "CMissionList_Charac/getData.inc"


#include "CMissionList_Charac/_saveData.inc"


#include "CMissionList_Charac/_reset.inc"


#include "CMissionList_Charac/Update_EnterWorld_event.inc"


#include "CMissionList_Charac/Update_Kill_event.inc"


#include "CMissionList_Charac/Update_Win_event.inc"


#include "CMissionList_Charac/Update_Revenge_event.inc"


#include "CMissionList_Charac/Update_GameOver_event.inc"


#include "CMissionList_Charac/Update_RecvPacket_event.inc"


#include "CMissionList_Charac/Update_ComboMission_Event.inc"


#include "CMissionList_Charac/Update_Within_Mission_event.inc"


#include "CMissionList_Charac/check_mission_kind.inc"


// ============================================================================
// 列表管理
// ============================================================================

#include "CMissionList_Charac/MakeMissionList_forNewbies.inc"


#include "CMissionList_Charac/MakeMissionList_forOldUser.inc"


#include "CMissionList_Charac/MakeMissionList_JustKind.inc"


#include "CMissionList_Charac/addNewMission.inc"


#include "CMissionList_Charac/Remove_Mission.inc"


#include "CMissionList_Charac/ClearMission.inc"


#include "CMissionList_Charac/ClearMissionKind.inc"


#include "CMissionList_Charac/ResetMissionList.inc"


#include "CMissionList_Charac/ResetDaily.inc"


#include "CMissionList_Charac/Reset_DailyMission.inc"


#include "CMissionList_Charac/GM_SetMissionValue.inc"


#include "CMissionList_Charac/getNewMission.inc"


// ============================================================================
// 发送
// ============================================================================

#include "CMissionList_Charac/Send_MissionList.inc"


#include "CMissionList_Charac/Send_Cleared_Mission_List.inc"


#include "CMissionList_Charac/Send_Fail_Mission.inc"


#include "CMissionList_Charac/setIssue.inc"


// ============================================================================
// 查询
// ============================================================================

#include "CMissionList_Charac/isClearMission.inc"


#include "CMissionList_Charac/getIndex_byKind.inc"


#include "CMissionList_Charac/getIssue_mode.inc"


#include "CMissionList_Charac/getIssue_all.inc"


#include "CMissionList_Charac/getWinningRate.inc"


#include "CMissionList_Charac/empty_progress.inc"


#include "CMissionList_Charac/isUserNewbie.inc"


#include "CMissionList_Charac/isClearStealingSkillMission.inc"

