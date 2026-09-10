// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CFairMatchRooms/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— fair_pvp::CFairMatch 重逻辑组（唯一 owner 本 TU）
// 依据 docs/class_func_reports/fair_pvp__CFairMatch/*.md（汇编=objdump -d，
// C=Ghidra 反编译）。每个函数定义上方标注 ORIG 地址与报告路径；凡无独立
// 报告仅凭 objdump 转写的行均已标【推断】。
//
// 字段映射（CFairMatch.h 布局契约固定不变）：
//   +0x004 m_field04        匹配状态机（0空闲/1排队/2超时兜底/3匹配中/
//                           4对战中/5局间）
//   +0x008 m_roomInfo       PVP_ROOM_INFO（title/byte4/short6/byte8/passwd）
//   +0x018 m_matchNotifyId  TimerStartMatching 附带 id（ctor 置 nextAuthValue_）
//   +0x01c m_teams[0]       TeamInfoThird（m_field00@+0/m_field04@+4/
//                           m_players[4]@+8，stride 0x48）
//   +0x064 m_teams[1]
//   +0x0ac m_fieldAC        OnConnectP2P 完成连接计数
//   +0x0b0 m_fieldB0        已完赛局数（OnReward ++，<3 允许续赛）
//   +0x0b4 m_fieldB4        _SetEnterCharacNo 写入的进场角色号
//   +0x0b8 m_leaderSeat[0]  队0 队长座位（OnMatchRoom 存 GetManagerSeat()；
//                           OnAskRematch 存队伍重赛决定，双用【推断】）
//   +0x0bc m_leaderSeat[1]  队1 队长座位（OnMatchRoom 尾 get_user_seat(房主)；
//                           双用同上）
//   +0x0c0 m_fieldC0        IMatch* next match（GetNextMatch/SetNextMatch 槽）
//
// 等待表交互均经 pWaitingRoomList_（IRoomList）虚槽：AddRoom/DelRoom/Match。
//
// ============================================================================
// b28f-SweepCFairMatchRooms（本波）判词汇总：四数 36/88/4/6 → 36/89/near=4/diff=5
//   （fresh 18:21:12）。逐件：
//   ① OnRematchTimeout：**真语义 bug 修复**（DIFF→AE，292=292）。ORIG 0x0856602b
//      `call CheckRematch; xor $0x1,%eax; test %al,%al; je 085662bb`，而
//      0x085662bb = `add $0x1c,%eax; call TeamInfoThird::refresh`（refresh 主体）
//      ⇒ 原码是**负极性写法 + 分支交换** `if (!CheckRematch(room)) { 队处理 }
//      else { refresh 主体 }`。原实现把 refresh 主体放进 `!CheckRematch` 分支
//      ⇒ CheckRematch 真/假两路处理**整体颠倒**（==1 该整场续战却去逐队拉出、
//      ==0 该拉出却续战）。同步修 `CheckRematch` 返回 int→bool（ORIG 调用点
//      bool 直推导数）⇒ `cmp $1; sete` 2 条 → `xor $1` 1 条。
//      收尾 1 处 `movl $0x0,0xc(%esp)` 位置差按 P211/P274 解：`_ReturnTeam(…, int
//      teamNo, …)` 形参 int 与局部 `unsigned int mgrTeam` 异型 ⇒ 先求值第 3 参。
//      残差 2 块 → 0（AE）；CheckRematch 本体 43=43 AE。
//   ② OnMatchRoom：残差 13 块全为槽号（N82 具名聚合优先低槽 / PacketGuard 落
//      -0x94 vs ORIG movable 占 -0x94）+ P93 同址别名；p167 CLEAR。
//   ③ UpdateScore：残差=帧 0x7c(我方)/0x6c + 深区槽差（N115 outgoing 预留 +
//      N82 分区）；语义块仅帧尺寸本身。
//   ④ CheckJoinable：`cmp %eax,%ebx; setge` vs `cmp %ebx,%eax; setle`（同一语义
//      的 compare 方向折痕，P196 折痕归一；两向源形实测同形）⇒ N196 封顶。
//   ⑤ OnWalkOut：五实参装载时刻 + 间接 call 目标 %eax/%edx（N7/N82附）。
//   ⑥ _PullOutRoomTeam：ORIG body 尾 `jmp <紧邻>` 空岛被 4.4 jump-thread（N121）。
//   ⑦ OnAskRematch：`this + idx*0x48 + 0x20` 的 lea 折叠 vs 我方两步 add（N107 族）。
//   ⑧ GiveTrainingSeal：残 1 条我方多 `jmp`（N121 epilogue 尾岛）。
//   ⑨ _ReturnTeam：两个 callee-saved 结果 ORIG esi/edi ↔ 我方 edi/esi 互换
//      （P75/N82附 跨版本调度；判据⑪ 判别为寄存器角色差非槽差）⇒ 封顶。
//   ⑩ diff 列表仍含 `_Z41__static_initialization_and_destruction_0ii`（N123 撞名伪差）。
//   ⇒ 本 TU 剩余真缺口 = 0（p167 全 CLEAR，仅 C1/C2、D1/D2 同址别名 P93 豁免）。
// ============================================================================
// b28d-solo-CFairMatchRooms（本波）判词汇总（逐件 fresh 复测，p167 全 CLEAR）
//   UpdateScore        DIFF→NEAR ndiff 133→32：P296 嵌套正向 if 消 m_flag 的
//                      xor $1 物化（ORIG `movzbl; test %al,%al; je` 直测）；
//                      P171 补 killCnt 死写入（ORIG `mov %eax,-0x44`）；
//                      P0 补 missionList 具名局部（ORIG `mov %eax,-0x1c`）；
//                      P3 cond 实参 `(unsigned char)` 收窄复现 `cmp $0x1,%al`。
//                      残差 = 深区临时槽深 0x10 + 帧 0x7c vs 0x6c（N115/N82）。
//   GiveTrainingSeal   DIFF ndiff 226→3：删 bool ok 改双短路（ORIG jle/jbe +
//                      mov$1/mov$0 汇聚）；bonusRemain bool→char（S1/P321-T3，
//                      复现 4.1 `cmpb $0x0,-0x11`）；P222 正向包裹翻块序（ORIG
//                      bonusRemain==0 块在尾）；P0 内联 getCurCharacInvenW（去
//                      inven 槽）；vector 声明序 updatedSlots 前置（ORIG 构造序
//                      -0x30→-0x3c）。残 3 = N121 epilogue 尾岛。
//   OnAskRematch       DIFF ndiff 244→36：删具名 bool reject 改 P226 语句表达式
//                      （复现 ORIG 无槽 `mov $1/mov $0` 岛 + test/je，槽整体回正
//                      -0x24/-0x20/-0x1c）；if 链→switch（P251/P286，复现形参
//                      载入 + `test;js;cmp$1;jle;cmp$2;je` 决策链）。残 = 伪差 +
//                      1 处 `add $0x20` vs `lea 0x20`（N107 族）。
//   OnRematchTimeout   DIFF ndiff 248→~90：P0 内联 CheckRematch 为最右 vararg
//                      （去 rematchable 槽）+ P3 `(unsigned char)` 收窄复现
//                      `movzbl %al,%edi`；P0 内联 room->m_relayMgr（去 relay 引用
//                      槽，帧 0x7c→0x6c 对齐）；team0Decision 具名槽→switch
//                      （P251/P261，复现 `test;je;test;js;cmp$2;jg`）。残 = 槽号
//                      分区 + `CheckRematch==1` 的 xor/sete（跨版本）+ N104 EH 段。
//   _ReturnTeam        NEAR 4（N）：CSwitchLog varargs 的 %esi/%edi 分配互换
//                      （ORIG 第1值→esi、4.4→edi），P75/N82附 跨版本不可择。
//   CheckJoinable      NEAR 4（N）：两处 `pc>=X`/`total>X` 被 4.4 折成反向比较
//                      （cmp %ebx,%eax;setle vs ORIG cmp %eax,%ebx;setge），
//                      两向源形实测同形（N196）。
//   OnWalkOut          NEAR 5（N）：五实参装载时刻 + 间接 call 目标 %ecx vs %eax
//                      （N7/N82附）；绑定形实测净负。
//   _PullOutRoomTeam   DIFF 2（N）：ORIG body 尾 `jmp <紧邻>` 空岛被 4.4
//                      jump-thread（N121）。
//   _CheckMovableRoom  DIFF 5（N18a）：`*out = (PvP_Room*)GetPVPRoom()` 的
//                      LHS-first 不可造；void** 视图形引 push %ebx（+3 条）实测
//                      净负回退。
//   OnMatchRoom        DIFF 187（N78/N82/N93）：ORIG nameBuf 落非 4 对齐槽
//                      -0x193 + 4.1 memset 对齐剥离慢路（N78）；matched/常量槽
//                      分区与帧 0x1ac vs 0x1bc（N82）；语义闭合（p167 50=50）。
// ============================================================================
// ============================================================================
#include "CFairMatch.h"

#include <algorithm>    // std::swap
#include <cstddef>

#include "PvP_Room.h"              // PvP_Room 全接口/CGameManager
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "CMissionList_Charac.h"
#include "MissionTypes.h"
#include "CFairPvPScore.h"
#include "CSwitchLog.h"
#include "GameTypes.h"             // cMyTrace
#include "LogManager.h"
#include "CDataManager.h"
#include "ServerParameterScript.h" // m_schoolMatchSeal / m_schoolMatchDailyPlayLimit
#include "GameWorld.h"
#include "TimerQueue.h"
#include "TimerDispatcher.h"
#include "CRelayBattleMgr.h"

using fair_pvp::CFairMatch;
using fair_pvp::TeamInfoThird;
using fair_pvp::PlayerInfoThird;

// CEloRating / PvPGlobalEnvironmentVariable 权威声明在 OnlinePreliminary.h
// （OnlinePreliminary 批维护；calc 定义在其 OnlinePreliminaryMatch.cpp）
#include "OnlinePreliminary.h"
#include "CGameManager.h"

// SPS 训练印章配置块视图（m_serverParameter 位于 CDataManager+0x68；
// ORIG 读 dm+0x49d8..0x49e0 = SPS+0x4970..0x497a；宏内联——P49）
#define FM_SPS() (G_CDataManager()->m_serverParameter)
// [dedup] moved to CFairMatch.cpp
// [dedup] moved to CFairMatch.cpp
int CFairMatch::nextAuthValue_ = 0;                       // B 0x0943e150
// [dedup] moved to CFairMatch.cpp

// TeamInfoThird 首字节视图（ORIG 以 *(char*)(this+team*0x48+0x1c) 判在座；
// 宏内联——P49：-O0 下 static inline helper 产真实 call 符号）
#define FM_TeamByte0(t) (*(reinterpret_cast<unsigned char*>(&(t))))


// ===================================================================== //
// 小函数组（ORIG 逐指令转写）
// ===================================================================== //

// _CheckSameTeam ORIG T 0x085629e8 报告 _CheckSameTeam.md —— 恒返 1
#include "CFairMatchRooms/_CheckSameTeam.inc"
}

// CheckUnusableEquipmentInUser ORIG T 0x08562cfe 报告同名 md —— 恒返 1（bool 返回：
// CheckJoinable +0x12/OnCreateRoom +0x11 均以 xor $1 取反，P22 实证）
#include "CFairMatchRooms/CheckUnusableEquipmentInUser.inc"
}

// OnGiveup ORIG T 0x08564de0 报告 OnGiveup.md —— 恒返 1
#include "CFairMatchRooms/OnGiveup.inc"
}

// OnWaitRematch ORIG T 0x0856649a 报告 OnWaitRematch.md —— 空 body
#include "CFairMatchRooms/OnWaitRematch.inc"
}

// GetEndPvPResultTimeout ORIG W 0x0856892a —— mov $0xea60
#include "CFairMatchRooms/GetEndPvPResultTimeout.inc"
}

// GetNextMatch / SetNextMatch / _SetEnterCharacNo ORIG W 0x08568934/42/54
#include "CFairMatchRooms/GetNextMatch.inc"
}
#include "CFairMatchRooms/SetNextMatch.inc"
}
#include "CFairMatchRooms/_SetEnterCharacNo.inc"
}

// _GetRating ORIG W 0x08568966 —— m_teams[team].GetTeamRating() 取 float
#include "CFairMatchRooms/_GetRating.inc"
}

// ===================================================================== //
// 检查组
// ===================================================================== //

// ORIG：while 形循环 seat<=7（cmpl $0x7/setle）统计 room->get_team(seat)==3 观战者；
// 末尾 size() 返回值以 ebx 缓存跨 call，cmp/setge 两分支：size-count >= playCount → 0，否则 1。
#include "CFairMatchRooms/_CheckObserver.inc"
}

// CheckJoinable ORIG T 0x08562a6e 报告 CheckJoinable.md
// b28d-solo-CFairMatchRooms 判词：N196——两处 `pc>=X`/`total>X` 被 4.4 折成反向
// 比较（cmp %ebx,%eax; setle/setl vs ORIG cmp %eax,%ebx; setge/setg，同语义不同
// compare 方向）；两向源码写法（`pc>=X` / `X<=pc`）实测同形 ⇒ 源形不可择。
// 余 1 条跳表重定位伪差。call 差集 8=8 CLEAR。fresh ndiff=4（85=85）。
#include "CFairMatchRooms/CheckJoinable.inc"
}

// OnReady ORIG T 0x08565018 报告 OnReady.md
#include "CFairMatchRooms/OnReady.inc"
}

// _CheckMatchable(const) ORIG T 0x08563a54 报告 _CheckMatchable.md
#include "CFairMatchRooms/_CheckMatchable.inc"
}

// _CheckRematchable(const) ORIG T 0x08563b16 报告 _CheckRematchable.md
#include "CFairMatchRooms/_CheckRematchable.inc"
}

// CheckMatchable ORIG T 0x08565c54 报告 CheckMatchable.md
// b28d-solo-CFairMatchRooms 判词：AE——myUser/otherUser 提函数头 + 循环变量 i 提头
// 共用同一槽（P212），逐条全等（.o 仅重定位标签差）。call 差集 0=0。
#include "CFairMatchRooms/CheckMatchable.inc"
}

// GetGap ORIG T 0x856491c 报告 GetGap.md —— 双 call 内联差：4.4 实测
// `__builtin_fabsf(f() - g())` 直出 `call; fstps -0xc; call; fsubrs -0xc; fabs`。
#include "CFairMatchRooms/GetGap.inc"
}

// idx∈[0,4)：ORIG 反汇编 0x856515c/0x856517d 实证槽基址 = this + 0x24 + idx*0x10
// （movzbl 0x18+(idx+1)*16 读 m_flag、+(idx+1)*16+0x14 读 m_user），即
// m_teams[0].m_players[idx]（m_teams[0] @ +0x1c，m_players @ +0x08）。
#include "CFairMatchRooms/GetUser.inc"
}

// GetPlayerCount ORIG T 0x8566d3e 报告 GetPlayerCount.md
#include "CFairMatchRooms/GetPlayerCount.inc"
}

// _FindTeam ORIG T 0x856663a8 报告 _FindTeam.md
#include "CFairMatchRooms/_FindTeam.inc"
}

// ===================================================================== //
// 房间生命周期组
// ===================================================================== //

// OnCreateRoom ORIG T 0x8562d08 报告 OnCreateRoom.md
// 装备检查通过 → 自增授权号、复位双队与计数；失败返回错误码 0x13
// （ORIG +0x18 mov $0x13 / +0xb7 mov $0x0；IMatch 基类返回 int）
#include "CFairMatchRooms/OnCreateRoom.inc"
}

// OnDestroyRoom ORIG T 0x8562dc6 报告 OnDestroyRoom.md
// [b28 BM2 判词·N7] AE-ndiff=6（fresh .o 归一）：pWaitingRoomList_->DelRoom 虚槽
// 调用 ORIG 4.1 以 %ecx 承载 fnptr、%edx/%eax 装载实参，4.4 恒 %eax 承载 call 目标
// （N7「间接 call 目标恒 %eax」，b26 NL28 族）。助记符/条数全等，纯寄存器文本差；
// 禁绑定形（P0/N96）。
#include "CFairMatchRooms/OnDestroyRoom.inc"
}

// OnJoinRoom ORIG T 0x8562df6 报告 OnJoinRoom.md
#include "CFairMatchRooms/OnJoinRoom.inc"
}

// _SendMatchStatePacket ORIG T 0x85651a6 报告 _SendMatchStatePacket.md
// msg=0xaf：state 广播给前 teamSpan 支队伍全员
#include "CFairMatchRooms/_SendMatchStatePacket.inc"
}

// _BackupRoomInfo ORIG T 0x8563674 报告 _BackupRoomInfo.md
#include "CFairMatchRooms/_BackupRoomInfo.inc"
}

// _RegisterWaitingRoom ORIG T 0x8563a32 报告 _RegisterWaitingRoom.md
// (**pWaitingRoomList_)->vtable[0] = AddRoom —— 经 IRoomList::AddRoom(this)
// [b28 BM2 判词·N7] 同 OnDestroyRoom：AddRoom 虚槽 %ecx vs %eax 承载（N7 族），
// 其余逐条同。
#include "CFairMatchRooms/_RegisterWaitingRoom.inc"
}

// _ChangeTeamLeader ORIG T 0x8563272 报告 _ChangeTeamLeader.md
// 离队者恰为队长座位 → 该队首名在座玩家接任并 0x13d 包通知全队
// b28d-solo-CFairMatchRooms 判词：AE——m_leaderSeat[2] 数组化（CFairMatch.h，偏移
// 0xb8/0xbc 与 sizeof 不变）+ 去 PlayerInfoThird 引用缓存，逐条全等。
#include "CFairMatchRooms/_ChangeTeamLeader.inc"
}

// OnLeaveRoom ORIG T 0x8562fbc 报告 OnLeaveRoom.md
// [b28 BM2 判词·N7] AE-ndiff=6（fresh .o 归一，全二进制口径 16 含标签伪差）：
// 唯一差 = _ReturnTeam 类虚槽调用 call *%ecx(ORIG) vs call *%eax(4.4) 及其
// 实参装载寄存器对调（N7 族，b26 NL28）。其余 181  insn 逐条同。
#include "CFairMatchRooms/OnLeaveRoom.inc"
}

// OnUnregisterMatch ORIG T 0x85663cc6（README 行：0x08563cc6）报告同名 md
// [b28 P222-y10 DIFF→NEAR] 否定合并前置形已翻正块序（ndiff 20→6）；残余 6 行 =
// DelRoom 虚槽 call *%ecx vs *%eax = N7 封顶。
// ORIG 返回 int：条件不满足 → 0；执行 → 1。
// [b28 BM2 判词·N7] AE-ndiff=8（fresh .o 归一）：DelRoom/Remove 虚槽 call *%ecx
// vs call *%eax + 实参 %edx/%eax→%ecx/%edx 对调（N7 族）。其余逐条同。
#include "CFairMatchRooms/OnUnregisterMatch.inc"
}

// OnRegisterMatch ORIG T 0x85623bba?（README 0x08563bba）报告同名 md
// ORIG 返回 int：m_field04==1 → 1；失败路径 → 0；成功 → 1。
#include "CFairMatchRooms/OnRegisterMatch.inc"
}

// ===================================================================== //
// 对局进行 / 换房组
// ===================================================================== //

// _CheckMovableRoom ORIG T 0x85663ed8?（README 0x08563ed8）报告同名 md
// [b28d-PoolSmall·检查点22 收尾] 唯一残差 `*out = GetPVPRoom()` 的 store 装载序
// （ORIG LHS-first `mov 0x14(%ebp),%edx; mov %eax,(%edx)` vs 4.4 RHS-first+edx
// 中转）由 **P211 档① callee 侧同型旋钮**解决：CUser.h `GetPVPRoom()` 返回
// void*→PvP_Room*（Main 检查点22 落地，mangled 不含返回类型、CUser.cpp:1128
// 本体同步）⇒ 目标与 RHS 同型，4.4 直出 LHS-first 直存。
// 【负例记录】绕冻结头曾用两种临时形，现均**不必要且更差**，勿再引入：
//  ①`*(void**)out = member->GetPVPRoom()` 同型视图——可翻 LHS-first 但引
//    `push %ebx` 缓存 out（+5 行，净负）；
//  ②`*out = static_cast<PvP_Room*>(...)` 冗余 cast——callee 同型后该 cast 不再
//    产生异型中转，实测与本形 TU 三数完全相同（36/88/3/7），即零收益且与 ORIG
//    源码形不符 ⇒ 已删（§4 真形优先）。
// call 差集 5=5 CLEAR。
#include "CFairMatchRooms/_CheckMovableRoom.inc"
}

// _PullOutRoom ORIG T 0x856633ce?（README 0x085633ce）报告同名 md
// b28d-solo-CFairMatchRooms 判词：IDENT——P227 全表达式临时链
// （`CSwitchLog(..)(...)`）翻正变参求值序（ORIG 在 ctor 前求 varargs 并跨 ctor
// 保活 edi/esi/ebx）；call 差集 10=10。fresh ndiff=0（66=66）。
#include "CFairMatchRooms/_PullOutRoom.inc"
}

// _PutInRoom ORIG T 0x856334bc?（README 0x085634bc）报告同名 md
// b28d-solo-CFairMatchRooms 判词：IDENT——P227 链式 + P162（put_byte(result) 不
// 收窄，消 movzbl）+ P89（去 guardAsBuf 局部，内联 reinterpret_cast）+ 成功分支内
// 独立 return seat（复现 ORIG 两处 ebx 物化）；call 差集 21=21。
// fresh ndiff=0（121=121）。
#include "CFairMatchRooms/_PutInRoom.inc"
}

// _MakeRoom ORIG T 0x8563692 报告 _MakeRoom.md
// b28d-solo-CFairMatchRooms 判词：IDENT——P227 链式 + P222 反向布局（ORIG
// `jle <失败尾块>` + result>=1 成功路径直落；改写 if(result>=1){成功}else{失败}）
// + P162；call 差集 45=45。fresh ndiff=0（230=230）。
#include "CFairMatchRooms/_MakeRoom.inc"
}

// _MoveRoom ORIG T 0x8563d32 报告 _MoveRoom.md
// 遍历 from 房 8 座逐个换到 to；观战队（team==3）换入后重设观战态
#include "CFairMatchRooms/_MoveRoom.inc"
}

// OnMatchRoom ORIG T 0x8564150 报告 OnMatchRoom.md。ORIG 失败路径 → 0；成功 → 1。
// b28d-BigFairMatch 终判：NEAR——orig 500 / ours 500（gap 0），13 个差异对全部只是栈槽号
// （movable ORIG -0x94↔我方 -0x88、PacketGuard ORIG -0x90↔我方 -0x94，其余 15 个槽
// 全等；P167 调用点差集仅 C1/C2、D1/D2 同址别名 = P93 豁免）。残差属 N82 族：4.4 对
// 具名聚合对象优先占低槽（PacketGuard 落 -0x94），4.1 按生命周期穿插（movable 占 -0x94）；
// 声明前置/嵌套块/分离赋值三形实测均劣化（分别 13→28/36/…）⇒ 封 N82。
// b28d-BigFairMatch 本轮逐段核对 ORIG 汇编 + Ghidra C，修 3 处真语义 bug + 12 处形态缺口：
//   ① 循环内 GetPlayerCount() receiver 是 this（ORIG `mov 0x8(%ebp),%eax; ... call *%edx`），
//      原写 static_cast<CFairMatch*>(it)->GetPlayerCount()（对 other 调用）——真 bug；
//   ② 循环内读 other->m_teams[0].m_players[p]（ORIG other+0x24+p*0x10 user /
//      +0x28 flag），原写 m_teams[1]（+0x6c）——真 bug（错队伍索引）；
//   ③ P3：`cmp $0x1,%al`（无 movzbl）⇒ setReamtchable 实参 uchar 收窄；
//   ④ P0/P168a：删具名 bool matched（ORIG `call; xor $1,%eax; test %al,%al; je` 无槽）；
//   ⑤ P171：ORIG 入口 `movl $0x0,-0x2c(%ebp)` 死写入照抄（全函数无读）；
//   ⑥ ORIG moveTarget(-0x80) 函数入口清零一次，循环内不重置；循环迭代变量直接复用
//      other 槽（-0x84，GetNextMatch 结果写回），无独立 it 槽；
//   ⑦ P171：nameBuf 255 字节（ORIG `mov $0xff,%ebx` + 4.1 对齐慢路），非 256；
//   ⑧ P171：DelRoom 无 pWaitingRoomList_ null 守卫（ORIG 直解引用）；
//   ⑨ P24/P0：删具名 timerKey（ORIG gen_timer_key 结果直落 %esi 跨 get_index/G_TimerQueue）；
//   ⑩ P183：删 `char* gb` 与 `PlayerInfoThird& s` 缓存（ORIG 每点重算地址）；
//   ⑪ 真 bug：_MoveRoom 实参方向反——ORIG 压栈 0x4(%esp)=room、0x8(%esp)=moveTarget
//      （Ghidra C `_MoveRoom(this,param_1,local_84,…)`）⇒ `_MoveRoom(room, moveTarget, …)`；
//   ⑫ P227/P87：CSwitchLog 改全表达式临时链 + 用 GetUserName 返回值（ORIG `mov %eax,%ebx`
//      跨 ctor 保活后 `mov %ebx,0xc(%esp)`，%s 实参是返回值非数组名）。
#include "CFairMatchRooms/OnMatchRoom.inc"
}

// ===================================================================== //
// 结算 / 重赛 / 奖励组
// ===================================================================== //

// _ReturnTeam ORIG T 0x85664dea?（README 0x08564dea）报告同名 md
// 把 room 内指定队成员退回各自新开房；excludedUser 不参与搬运
// b28d-solo-CFairMatchRooms 判词：N7——P227 链式 + P196（seat unsigned 复现
// setbe）后仅剩 CSwitchLog varargs 的 esi↔edi 分配互换（ORIG 第1值→esi/第2→edi，
// 4.4 反之）+ 对应压栈序（4 行 NEAR，113=113）。跨版本 callee-saved 分配不可择
// （P75/N82附）⇒ 封顶。call 差集 11=11 CLEAR。
#include "CFairMatchRooms/_ReturnTeam.inc"
}

// _PullOutRoomTeam ORIG T 0x85665d86?（README 0x08565d86）报告同名 md
// b28d-solo-CFairMatchRooms 判词：N121——P227 链式 + P225/P295（while+break 复现
// `cmp $2; sete; test; je`）后仅缺 ORIG body 尾 `jmp <紧邻下一指令>`（1 行）：
// 4.4 恒 jump-thread 掉该空岛（N121 epilogue-threading 尾岛不可消）⇒ 封顶。
// fresh ndiff=1（157/156）。call 差集 10=10 CLEAR。
// [b28d-PoolSmall 复验] 再补两形实测均不闭合：①`break`→`goto <体尾空标签>`
// （标签置于 while 之后）⇒ 4.4 仍把 goto 融为 fall-through，`jmp` 不发射
// （l8_cmp 严格口径 ndiff 2，其中 1 行为 rodata 标签伪差）；②UpdateScore 侧
// P212「全头部声明 + 块内仅赋值」把 killCnt 提出块外 ⇒ 深槽号零改善、
// 本函数 ndiff 6→8 反劣（已回滚）。⇒ P223 定案③「4.4 岛数 = 条件数或 0」
// 成立，N121 封顶维持。
#include "CFairMatchRooms/_PullOutRoomTeam.inc"
}

// OnWalkOut ORIG T 0x8564fb2 报告 OnWalkOut.md。ORIG 返回 1。
// 转发到本类虚槽 +0x2c（OnLeaveRoom(user,room,team,seat)）
// b28d-75（N82附/P75 封顶，语义等价）：vtable[+0x2c] 虚调用目标寄存器两侧同为
// %ecx（b28d 复核，旧 N7 判词已不适用）；唯一真差 = 五实参装载时刻：
// ORIG 4.1 先载第4参 team(-0x10)→%eax，再 seat→0x10；4.4 先载第2参
// userSeat(-0xc)→%eax。b28d 复测 (int)team 冗余 cast 形无变化（P41 在此不触发）
// ⇒ 跨版本实参装载调度，禁绑定形（P0）⇒ N82附 封顶。
// b28d-solo-CFairMatchRooms 判词：N82附——五实参物化序差：ORIG 先物化 team
// (-0x10)→%eax 再压 seat/team/room/userSeat；4.4 先物化 userSeat(-0xc)→%eax、
// team/room 走 %edx。unsigned team / `*&team` 两形实测均引 push %ebx（fnptr 落
// %ebx 而非 ORIG %ecx）⇒ 净负回退。槽位集全等、call 差集 2=2 CLEAR。封顶。
// fresh ndiff=5（33=33 NEAR）。
#include "CFairMatchRooms/OnWalkOut.inc"
}

// CheckRematch ORIG T 0x85665d04 报告 CheckRematch.md
// [b28f-SweepCFairMatch] 返回 bool（P248 档①/P292）：ORIG 本体各路径 `mov $0x1/%0x0,%eax`
// 直返、调用点 `xor $1; test %al` 直推导数 ⇒ 原码 bool；0/1 → false/true 语义等价。
#include "CFairMatchRooms/CheckRematch.inc"
}

// OnEndFight ORIG T 0x8565048 报告 OnEndFight.md
// b28d-solo-CFairMatchRooms 判词：IDENT——P211/P274（mgrTeam 局部 unsigned 与
// _ReturnTeam int 形参异型，翻实参装载序）+「声明即初始化」形（`PvP_Room*
// releasedRoom = _ReturnTeam(...)` 令 mgrTeam 深槽 -0x20/返回值浅槽 -0x1c，
// 探针 /tmp/fm_p4.cpp c/d）+ P83 结构赋值；call 差集 9=9。fresh ndiff=0（71=71）。
#include "CFairMatchRooms/OnEndFight.inc"
}

// OnReward ORIG T 0x85665966?（README 0x08565966）报告 OnReward.md
#include "CFairMatchRooms/OnReward.inc"
}

// UpdateScore ORIG T 0x8565700 报告 UpdateScore.md
// 遍历 teamIdx 队 4 槽在座玩家逐个结算；win/lose/result 即报告 param5/6/7。
// info 仅取 m_job(+5)/m_growType 低 nibble(+6)（ORIG asm 读 info+5/+6）。
// b28d-solo-CFairMatchRooms 判词：DIFF→NEAR（ndiff 133→32，172→174 指令）。
// 已收：P296 嵌套正向 if（`if (m_flag) { if (user) { body } }`）复现 ORIG
// `movzbl; test %al,%al; je` 直测（取反 continue 形 4.4 恒 xor $1 物化）；
// P171 补 killCnt 死写入数组（ORIG 0x8565904 `mov %eax,-0x44`，P232 未用数组
// 仍占槽）；P0 补 missionList 具名局部（ORIG 0x85658ca `mov %eax,-0x1c`，内联形
// 把 GetCharacExpandData(8) 排到 killCnt 之后）；P3 cond 实参
// `(unsigned char)` 收窄复现 `cmp $0x1,%al`；P227 链式、P191 双初始化、P133 两
// 独立守卫、P157/P183 去引用缓存、score/missions 内联。真语义已修（job/grow 取
// 第 2 参 info）。残差 = 深区临时槽（score7/grow）深 0x10 + 帧 0x7c vs 0x6c
// （N115 4.4 outgoing 预留 + N82 槽分区）；call 差集 17=17 CLEAR。
#include "CFairMatchRooms/UpdateScore.inc"
}

// GiveTrainingSeal ORIG T 0x85652c8 报告 GiveTrainingSeal.md
// b28d-solo-CFairMatchRooms 判词：DIFF（ndiff 226→3，274→276 指令；全部槽位
// 与 ORIG 对齐）。已收：删具名 bool ok 改双短路（ORIG 0x0856564e..0x08565675
// `jle`/`jbe` + mov$1/mov$0 汇聚）；bonusRemain bool→char（S1 一次性旋钮，
// P321-T3 消费端 `cmpb $0x0,-0x11; je` 无 setcc ⇒ bool/char 直证不足，char 复现
// 4.1 内存直比；bool 恒 xor 物化 N122）；P222 正向包裹翻块序（ORIG bonusRemain==0
// 块在尾部、win 链 fall-through）；P0 内联 `getCurCharacInvenW()`（ORIG 无 inven
// 槽，结果直留 %eax 跨实参装载）；vector 声明序 updatedSlots 前置（ORIG 构造序
// -0x30→-0x3c、push_back this=-0x3c）；P183 去 sps/score 缓存、P3 `cmp $1,%al`、
// dailyLimit 表达式内联、P135 push_back(O&&)。残 3 = N121 epilogue 尾岛
// （我方多 `jmp+nop`）。p167 49=49 CLEAR。
#include "CFairMatchRooms/GiveTrainingSeal.inc"
}

// OnRematchTimeout ORIG T 0x85665f92 报告 OnRematchTimeout.md
// b28d-solo-CFairMatchRooms 判词：DIFF（ndiff 248→~90，295→292 指令；帧
// 0x7c→0x6c 与 ORIG 对齐）。已收：P0 内联 CheckRematch 为最右 vararg（ORIG 无
// rematchable 槽，`call; movzbl %al,%edi` 直留 callee-saved）+ P3 `(unsigned char)`
// 收窄复现 movzbl；P0 内联 `room->m_relayMgr.OnReady`（去 relay 引用槽，帧回正）；
// team0Decision 具名槽→`switch (m_teams[0].m_field04)`（P251/P261，复现 ORIG
// `test %eax,%eax; je; test; js; cmp $2; jg` 无槽决策链）。残 = 槽号分区（pkt/kept/
// mgrTeam 深 4~0xc，N82）+ `CheckRematch(room)==1` 的 xor vs cmp/sete（跨版本）+
// N104 EH 尾块相对序；call 差集 46=46 CLEAR。
#include "CFairMatchRooms/OnRematchTimeout.inc"
}

// OnAskRematch ORIG T 0x856664a0 报告 OnAskRematch.md
// param：0=拒绝 1=接受 2=全队同意提交；队决定写入 m_field04(team)
// b28d-solo-CFairMatchRooms 判词：DIFF（ndiff 244→36，322→321 指令；room/myTeam/
// other/trace/pkt 槽全部回正 -0x24/-0x20/-0x1c/-0x34/-0x40）。已收：删具名 bool
// reject 改 P226 语句表达式 `if (({ !(A && B && C); }))`（ORIG 0x08566795/0x0856679c
// 无槽 `mov $1/mov $0` 岛 + test/je；具名 bool 曾多占 -0x19 槽把全帧推深 4）；if
// 链→`switch (decision)`（P251/P286，复现 ORIG `mov 0x10(%ebp),%eax; test; js;
// cmp $1; jle; cmp $2; je; jmp` 形参载入 + 寄存器比较链）；P227 链式。残 = 伪差 +
// 1 处 `m_teams[myTeam].m_field04=decision` 的 `add $0x20` vs `lea 0x20`（N107 族）；
// call 差集 41=41 CLEAR。
#include "CFairMatchRooms/OnAskRematch.inc"
}

// OnEndPvPResult ORIG T 0x8566694c 报告 OnEndPvPResult.md
// b28d-solo-CFairMatchRooms 判词：IDENT——P227 链式 + P0 内联
// `_ReturnTeam(room, _GetOppositeTeam(teamOf), 0)`（去 oppo 具名槽使全帧槽号回正）
// + P221 链式 trace；call 差集 29=29。fresh ndiff=0（267=267）。
#include "CFairMatchRooms/OnEndPvPResult.inc"
}

// ===================================================================== //
// 载入 / P2P 组
// ===================================================================== //

// OnCompleteLoad ORIG T 0x8564962 报告 OnCompleteLoad.md（msg 0x116）
#include "CFairMatchRooms/OnCompleteLoad.inc"
}

// OnConnectP2PTimeout ORIG T 0x8564d3a 报告 OnConnectP2PTimeout.md
// [b28 判词·N] AE-ndiff=2：唯一真差 = _ReturnTeam 实参装载中 mgrTeam 槽 reload 与
// `movl $0x0,0xc(%esp)` 的先后交换——4.1 值先载、4.4 常量先写（P75/N82附 装载时刻
// 跨版本调度）。探针 /tmp/l2_p3.cpp w1-w7（具名/拆分/语句表达式/cast 全形）不可翻；
// 禁 register 绑定（P0/N96）。其余逐条同。
// b28d-solo-CFairMatchRooms 判词：IDENT——P211/P274：局部 unsigned mgrTeam 与
// _ReturnTeam 的 int 形参异型，令 4.4 先求值第 3 参再写第 4 参常量（探针
// /tmp/fm_p3.cpp w31）；call 差集 5=5。fresh ndiff=0（52=52）。
#include "CFairMatchRooms/OnConnectP2PTimeout.inc"
}

// OnConnectP2P ORIG T 0x8564a8e 报告 OnConnectP2P.md
// 判例（ADeepY4[4]）：NEAR 196=196（原 DIFF 204）。已收：连接探测循环改 ORIG 形
//   `for(i=0;(i<8)&&(i<port);++i){if(addr[i]=='\0'){_SendMatchStatePacket(4,2);OnEndFight(room);return;}}`（4.4 收敛 ORIG 底部
//   跳转循环布局）、mgrTeam 局部内联直参、GetPlayCountPerOneTeam()&0xff→(unsigned char) 复现
//   movzbl、m_fieldAC==比较复现 sete；双层队员循环去 PlayerInfoThird& 引用、hoist m_user 读序
//   （ORIG 先读 m_user 存槽再判 m_flag，Send 用槽）。残余=+0x40 兜底虚槽调用 ORIG 用 %edx
//   承载 fn、我们用 %eax（寄存器文本差 _NEAR_）+ 双层索引 ORIG 用 lea 链、我们用更紧凑寻址
//   （计数已对称）。
// [b28 判词·N] AE-ndiff=6：残余全为 +0x40 兜底虚槽 call *%edx(ORIG) vs call *%eax
// (4.4) = N7（间接 call 目标恒 %eax，b26 NL28 8 件实证族）。其余逐条同。
#include "CFairMatchRooms/OnConnectP2P.inc"
}


// ORIG 反汇编实证：teamNo==1 分支 GetTeamRating 用 this+0x64（m_teams[1]），
// 队员循环读 this+0x24+i*0x10（m_teams[0]）；else 分支相反（0x1c / 0x6c）。
// BLOCKED 局部依赖：CEloRating::calc 本体与 PvPGlobalEnvironmentVariable::
// school_match_elo_k_ 定义尚未由任何 TU 提供（仅 ORIG 符号）。
// b28d-solo-CFairMatchRooms 判词：AE——去 PlayerInfoThird 引用缓存 + memberRating/
// oppositeRating 声明序翻转，逐条全等；仅剩 elo_k 全局重定位伪差。
// call 差集 6=6 CLEAR。
#include "CFairMatchRooms/_UpdateRating.inc"
}
