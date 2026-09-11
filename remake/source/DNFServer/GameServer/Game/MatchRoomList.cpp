// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MatchRoomList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <cstdio>

#include "MatchRoomList.h"
#include "CUser.h"
#include "CFairMatch.h"   // fair_pvp::CFairMatch 完整定义（原嵌套 stub 已迁）
#include "LogManager.h"

// ---- CSwitchLog（PvP 日志；与 PvP_Room.cpp 同款局部声明） ----
class CSwitchLog
{
public:
    CSwitchLog(const char* name, int line, int a, int b);
    void operator()(const char* fmt, ...);

private:
    char m_pad[0x10];
};

// ============================================================================
// CWaitingRoomList
// ============================================================================

#include "MatchRoomList/CWaitingRoomList.inc"


#include "MatchRoomList/dtor_CWaitingRoomList.inc"


#include "MatchRoomList/find.inc"


#include "MatchRoomList/erase.inc"


#include "MatchRoomList/insert.inc"


#include "MatchRoomList/doMatch.inc"


// ============================================================================
// CLeagueMatchRoomList
// ============================================================================

#include "MatchRoomList/AddRoom.inc"


#include "MatchRoomList/DelRoom.inc"


#include "MatchRoomList/Match.inc"


// ============================================================================
// CMultiMatchRoomList
// ============================================================================

#include "MatchRoomList/CMultiMatchRoomList.inc"


#include "MatchRoomList/AddRoom_1.inc"


#include "MatchRoomList/DelRoom_1.inc"


#include "MatchRoomList/Match_1.inc"


#include "MatchRoomList/GetWaitingRoomCombination.inc"


// ============================================================================
// fair_pvp::CFairMatch::GetPlayCountPerOneTeam（ORIG 0x08562834 恒返 1）
// 由 MatchRoomLCB.cpp 唯一定义；GetUserName 及三类房间表方法为
// MatchRoomList.cpp 唯一 owner（key function → vtable 0x8ca3ba8/0x8ca3bc0）。
// ============================================================================
