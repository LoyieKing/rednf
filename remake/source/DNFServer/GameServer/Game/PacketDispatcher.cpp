// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PacketDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "PacketDispatcher.h"
#include "Dispatchers.h"           // 具体业务 Dispatcher_* 完整类型
#include "GlobalData.h"            // s_msgq_mgr / s_antibotChecker_ / s_systemTime_
#include "DisPatcherCore.h"         // 本批补齐的 79 个 DisPatcher_*/ARAD 等声明
#include "MsgQueueMgr.h"
#include "CAppLoadChecker.h"       // 叶子批次（ORIG CAppLoadChecker 簇）
#include "PacketTracer.h"          // 叶子批次（CPacketTracerInstance/AddTcpLog）
#include "PacketSecurity.h"        // 叶子批次（DenyPacket/_NS_PI_Check_CMD/
                                   //  _NS_PI_2ND_MakeChecksumTo1Byte2）
#include "UserPacketCounter.h"     // message_counter::CUserPacketCounter
#include "Encryption.h"            // Encryption::crc32N
#include "CSystemTime.h"           // getCurTickCount/update
#include "GameWorld.h"             // G_GameWorld()->GetChannelType 等
#include "CGameManager.h"          // G_CGameManager()->getUser
#include "LogManager.h"
#include "cHistoryTrace.h"         // cMyTrace
#include <string.h>
#include "CEnvironment.h"    // GlobalInstance<T>

char* NumberToString(unsigned int value, int radix);  // ORIG _Z14NumberToStringji

// ======== CLocalChinaErrorDispatcher ========

#include "PacketDispatcher/CLocalChinaErrorDispatcher.inc"


#include "PacketDispatcher/_PacketErrorCheck.inc"


#include "PacketDispatcher/dtor_CLocalChinaErrorDispatcher.inc"

#include "PacketDispatcher/ProcAntiAddiction.inc"
#include "PacketDispatcher/ProcSafeModeUser.inc"
#include "PacketDispatcher/ProcSaefModeAfterLogin.inc"
#include "PacketDispatcher/ProcLockInSeriaRoom.inc"

#include "PacketDispatcher/doErrorDispatcher.inc"


// ======== PacketDispatcher ========


message_counter::CUserPacketCounter* PacketDispatcher::pUserPacketCounter_ = 0;
    // ORIG B 0x9472d18：静态指针；ORIG 由 DispatchThread::dispatch(0x847e2d2) 每轮
    // 以栈上局部 CUserPacketCounter(get_file_name(), "UserPacketCounter") 地址赋值（0x847e34f 实证）。

#include "PacketDispatcher/PacketDispatcher.inc"


#include "PacketDispatcher/dtor_PacketDispatcher.inc"


#include "PacketDispatcher/get_dispatcher.inc"


#include "PacketDispatcher/getDispatcher.inc"


#include "PacketDispatcher/log_recv_packet.inc"


#include "PacketDispatcher/server_load_monitoring.inc"


#include "PacketDispatcher/dispatch.inc"


#include "PacketDispatcher/doDispatch.inc"


// ---- 全局单例访问器（ORIG W 0x0823453e）----
PacketDispatcher* G_PacketDispatcher()
{
    return GlobalInstance<PacketDispatcher>::inst_ptr();
}
