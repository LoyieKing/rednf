// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W6_IPGDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"

#include "GameTypes.h"        // Guard<Mutex> / Mutex / DynamicPool
#include "TaiwanCash.h"       // WongWork::IPG 权威类
#include "LogManager.h"
#include "GlobalData.h"       // s_stream_pool / s_msgq_mgr / s_server_stop
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "CEnvironment.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// ORIG T 0x0858c4e0（_ZN7OS_API5SleepEi，nm/objdump 实测）：真实定义已由
// LinkFinalDeps.cpp 落地（usleep 包装）；此处声明与 CInventory.h 权威一致。
namespace OS_API
{
void Sleep(int ms);
int snprintf(char* buf, unsigned int n, const char* fmt, ...);  // 同 CInventory.h 权威声明
}

// ORIG T（_Z14NumberToStringyi）：64 位版 NumberToString；32 位版声明在
// CInventory.h。定义属 NumberToString 权威 TU，此处仅声明。
// _SetIPGData 的 get_acc_id()（unsigned int）须选 u32 版（ORIG _Z14NumberToStringji），
// 故两重载都声明；tranNo 为 ull 仍走 64 位版。
char* NumberToString(unsigned long long value, int radix);
char* NumberToString(unsigned int value, int radix);


namespace WongWork { namespace IPG {

#include "W6_IPGDeps/getIPGStatus.inc"

#include "W6_IPGDeps/stIPGStatus_t.inc"

// ---------------------------------------------------------------------------
// CIPGMgr —— 数据池与请求队列
// ---------------------------------------------------------------------------

#include "W6_IPGDeps/Finalize.inc"

#include "W6_IPGDeps/AllocIPGData.inc"

#include "W6_IPGDeps/FreeIPGData.inc"

#include "W6_IPGDeps/PutIPGRequest.inc"

#include "W6_IPGDeps/_GetIPGRequest.inc"

#include "W6_IPGDeps/getIPGStatus_1.inc"

// ---------------------------------------------------------------------------
// CIPGMgr::CIPGWorker —— 线程体与同步处理链
// ---------------------------------------------------------------------------

#include "W6_IPGDeps/ProcIPGSync.inc"

#include "W6_IPGDeps/ProcIPGSyncForChina.inc"

#include "W6_IPGDeps/dispatch.inc"

#include "W6_IPGDeps/ProcIPGSyncForTaiwan.inc"

// ---------------------------------------------------------------------------
// CIPGMgr::CIPGWorker::CIPGNetwork::ConnectTo —— ORIG T 0x081043e0 (0xb4)：
// socket(AF_INET,SOCK_STREAM,0) → connect(inet_addr/htons)，失败码 -1/-2。
// socket/connect/inet_addr/htons 经 PLT（同 ORIG），加 <sys/socket.h 等 include 见尾注。
// ---------------------------------------------------------------------------
} }  // namespace WongWork::IPG

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace WongWork { namespace IPG {

#include "W6_IPGDeps/ConnectTo.inc"


// ---------------------------------------------------------------------------
// CIPGHelper —— 报告 docs/class_func_reports/WongWork__IPG__CIPGHelper/
// ---------------------------------------------------------------------------

#include "W6_IPGDeps/Initialize.inc"

#include "W6_IPGDeps/Finalize_1.inc"

#include "W6_IPGDeps/GetIPGMgr.inc"

#include "W6_IPGDeps/_SetIPGData.inc"

#include "W6_IPGDeps/IPGRequest.inc"

#include "W6_IPGDeps/IPGQuery.inc"

#include "W6_IPGDeps/IPGInput.inc"

#include "W6_IPGDeps/IPGInput_1.inc"

#include "W6_IPGDeps/IPGInputCancel.inc"

#include "W6_IPGDeps/IPGInputPoint.inc"

#include "W6_IPGDeps/IPGOutput.inc"

#include "W6_IPGDeps/IPGOutputPoint.inc"

#include "W6_IPGDeps/IPGOutputSimulation.inc"

#include "W6_IPGDeps/IPGOutputCancel.inc"

#include "W6_IPGDeps/IPGOutputCancel_1.inc"

#include "W6_IPGDeps/IPGOutputConfirm.inc"

#include "W6_IPGDeps/IPGOutputConfirm_1.inc"

#include "W6_IPGDeps/IPGResult.inc"

} }  // namespace WongWork::IPG
