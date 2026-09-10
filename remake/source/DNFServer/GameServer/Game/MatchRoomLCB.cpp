// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 MatchRoomLCB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— 匹配备份域批次 B（LinkClosureB.MatchRooms 独占 TU）
// 承载符号（ORIG /home/wangyilei/下载/df_game_r nm/objdump 实证）：
//   1. CLeagueMatchRoomList::CLeagueMatchRoomList()
//        W _ZN20CLeagueMatchRoomListC1Ev @0x08568fb0
//        （vtable 写入 0x8ca3bb0，基类 IRoomList ctor 0x8568fa2）
//   2. CMultiMatchRoomList::CMultiMatchRoomList()
//        W _ZN19CMultiMatchRoomListC1Ev @0x08566efc
//        （vtable 写入 0x8ca3bc8；+0x04 起 std::map<int,CWaitingRoomList>，
//          按 GetPlayCountPerOneTeam()&0xff 预建等待列表）
//   3. fair_pvp::CFairMatch::GetPlayCountPerOneTeam()
//        T _ZN8fair_pvp10CFairMatch22GetPlayCountPerOneTeamEv @0x08562834
//        （ORIG 体：push/mov %esp,%ebp/mov $0x1,%eax/pop %ebp/ret —— 恒返 1）
//   4. online_preliminary::COnlinePreliminary::_ReturnTeam(PvP_Room*,int,CUser*)
//        T _ZN18online_preliminary18COnlinePreliminary11_ReturnTeamEP8PvP_RoomiP5CUser
//        @0x08561e40（objdump 逐指令转写，尾部 this+0x4=0）
// 报告依据：docs/class_func_reports/{CLeagueMatchRoomList,CMultiMatchRoomList,
//   fair_pvp__CFairMatch/_ReturnTeam.md,online_preliminary__COnlinePreliminary}。
// ============================================================================
#include <utility>

#include "MatchRoomList.h"      // CLeagueMatchRoomList / CMultiMatchRoomList / IRoomList / IMatch
#include "CFairMatch.h"         // fair_pvp::CFairMatch::GetPlayCountPerOneTeam 声明
#include "OnlinePreliminary.h"  // online_preliminary::COnlinePreliminary 声明（_ReturnTeam 等）
#include "CUser.h"
#include "PvP_Room.h"           // get_user_seat/get_team/OnSetPVPSeatState

// ---------------------------------------------------------------------------
// CLeagueMatchRoomList / CMultiMatchRoomList 构造函数（ORIG 0x08568fb0 /
// 0x08566efc）连同 AddRoom/DelRoom/Match key function（ORIG vtable
// 0x8ca3ba8 / 0x8ca3bc0）由 MatchRoomList.cpp 唯一定义；此处不重复，
// 避免同符号多重定义。
// ---------------------------------------------------------------------------

namespace fair_pvp
{

// ---------------------------------------------------------------------------
// 3. CFairMatch::GetPlayCountPerOneTeam —— ORIG 0x08562834 恒返 1。
//    [b28d-solo-MatchRoomLCB] N 封顶（§6.1 nop 豁免 + 切片伪差，源码侧无可动）：
//    ORIG nm size 0x15 = 函数体 5 条（+0x00..+0x09，与本实现逐字全等：
//    push/mov/mov$1/pop/ret，我方 .o size 0x0a）+ 10×nop 对齐（+0x0a..+0x13）
//    + 孤立 ret（+0x14，位于本符号范围内、下一函数 CFairMatch::C1Ev 0x856284a 之前）。
//    4.4 对 `return 1;` 不可能在 ret 后再发 nop×10+ret ⇒ 切片伪差不可消；
//    p167 调用点差集 CLEAR（0/0）；Ghidra C 亦为 `return 1;`。
// ---------------------------------------------------------------------------
#include "MatchRoomLCB/GetPlayCountPerOneTeam.inc"
}

}  // namespace fair_pvp

namespace online_preliminary
{

// ---------------------------------------------------------------------------
// 4. _ReturnTeam —— ORIG 0x08561e40 逐指令转写。
//    ORIG 帧：-0x18 = seat；-0x14 = newRoom；-0x10 = seatIdx；-0xc = team。
// ---------------------------------------------------------------------------
// [b28d-solo-MatchRoomLCB] AE（check_tu_routed: ae=1；.o size 0x125 = ORIG 0x125）。
//   ORIG 0x08561e40 逐指令转写（79 insn；残差仅 .o 重定位标签伪差，指令偏移
//   +0x10a/+0xff/+0x102/+0x91/... 逐一对应）。p167 CLEAR（6/6）。
//   槽位：-0x18 seat / -0x14 newRoom / -0x10 seatIdx / -0xc curTeam。
//   真语义修正（ORIG 实证）：get_user_seat 单次调用并缓存 seat，四处下游调用
//   （OnSetPVPSeatState/_PullOutRoom/_MakeRoom/_PutInRoom）第 1/2 实参均为
//   **seat**（ORIG 压 -0x18）而非形参 user；旧稿误传 user 且把
//   `team==curTeam && team!=2` 并入 else 分支（ORIG +0x60 jne 岛2 = continue）。
//   seatIdx 为 unsigned（ORIG 尾测 setbe），仅在 for 内单次初始化（ORIG 头
//   只有一条 movl $0x0,-0x10 —— 头部再声明一条会多 1 条 DIFF）。
#include "MatchRoomLCB/_ReturnTeam.inc"
}

}  // namespace online_preliminary

