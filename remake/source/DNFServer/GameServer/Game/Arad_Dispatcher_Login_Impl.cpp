// ============================================================================
// [SplitC 2026-09-11 batch2 · TU 迁移（ORIG GI 锚点）] 本文件由 ARAD_LoginSecurity.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN4ARAD26Arad_Dispatcher_Login_ImplC2Ev
//   ORIG TU     ：Arad_Dispatcher_Login_Impl.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include "ARAD_LoginSecurity.h"
#include <cstring>
#include <time.h>
#include "CDataManager.h"      // G_CDataManager / PvPChannel 等级接口（W 符号）
#include "CGM_Manager.h"
#include "CSimpleSSO.h"
#include "CRijndael.h"
#include "DB_Login.h"          // DB_Login::tagDNFEventEntry 完整类型
#include "DBMgr.h"
#include "GlobalData.h"
#include "GameWorld.h"
#include "LogManager.h"
#include "PacketBuf.h"         // G_GameWorld（与 GameWorld.h 同源声明）
#include "Stream.h"
#include "StreamDeps.h"        // G_CEnvironment

namespace ARAD
{
#include "Arad_Dispatcher_Login_Impl/Arad_Dispatcher_Login_Impl.inc"
}  // namespace ARAD
