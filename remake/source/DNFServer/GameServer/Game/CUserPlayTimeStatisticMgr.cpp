// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUserPlayTimeStatisticMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CUserPlayTimeStatisticMgr —— 在线用户游玩时长统计实现。
// 报告：docs/class_func_reports/CUserPlayTimeStatisticMgr/{InsertUserPlayTime,UpdateUserPlayTime}.md
// GetInstanceUserPlayTimeStatisticMgr 由 objdump 0x0860cba5 转写（__cxa_guard 单例）。
#include "CUserPlayTimeStatisticMgr.h"

#include <cstring>

#include "CStreamGuard.h"
#include "GlobalData.h"     // s_stream_pool / s_msgq_mgr
#include "MsgQueueMgr.h"
#include "SigTypes.h"       // SIG_USER_PLAY_TIME_STATISTIC（SigTypes_E.h 分片）
#include "StreamPool.h"
#include "CEnvironment.h"   // G_CEnvironment / get_channel_no

#include <cstdio>           // sprintf
#include <cstdlib>          // atoi
#include <string>

// NumberToString（ORIG T 0x081090b4；权威声明 CInventory.h）
char* NumberToString(unsigned int value, int radix);

#include "CUserPlayTimeStatisticMgr/CUserPlayTimeStatisticMgr.inc"


#include "CUserPlayTimeStatisticMgr/dtor_CUserPlayTimeStatisticMgr.inc"


#include "CUserPlayTimeStatisticMgr/InsertUserPlayTime.inc"


#include "CUserPlayTimeStatisticMgr/UpdateUserPlayTime.inc"


CUserPlayTimeStatisticMgr* GetInstanceUserPlayTimeStatisticMgr()
{
    // ORIG 0x0860cba5：__cxa_guard 单例（instance 0x09473444，析构经 __cxa_atexit）。
    static CUserPlayTimeStatisticMgr instance;
    return &instance;
}

#include "CUserPlayTimeStatisticMgr/CreateUserPlayTimeQuery.inc"


#include "CUserPlayTimeStatisticMgr/ConvertHHMMStringToInt.inc"

