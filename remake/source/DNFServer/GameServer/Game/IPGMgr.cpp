// ============================================================================
// [SplitC 2026-09-11 batch2 · TU 迁移（ORIG GI 锚点）] 本文件由 W6_IPGDeps.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN8WongWork3IPG7CIPGMgr10CIPGWorker11CIPGNetwork9ConnectToEPKct
//   ORIG TU     ：IPGMgr.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace WongWork
{
namespace IPG
{
#include "IPGMgr/ConnectTo.inc"
}  // namespace IPG
}  // namespace WongWork
