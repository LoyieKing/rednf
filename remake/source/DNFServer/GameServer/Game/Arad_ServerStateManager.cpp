// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_ServerStateManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ ARAD::Arad_ServerStateManager 实现（13 方法全域还原批）。
// 报告：docs/class_func_reports/ARAD__Arad_ServerStateManager/（汇编+反编译 C 全读）。
#include "Arad_ServerStateManager.h"

#include <algorithm>
#include <cstring>

#include "Arad_InternalDispatcher.h"  // make_internal_stream_jpn / K_*_SERVER_STATE
#include "CStreamGuard.h"             // CStreamGuard(Stream*,bool) / put_binary / dtor
#include "StreamPool.h"
#include "MoreTypes.h"               // arad::SigServerState 完整定义

#include "GlobalData.h"               // s_stream_pool / s_msgq_mgr / s_systemTime_
#include "MsgQueueMgr.h"              // MsgQueueMgr::DB_Q
#include "GameWorld.h"                // G_GameWorld / send_all
#include "CUser.h"                    // Send
#include "PacketGuard.h"              // 0x21a 效果包
#include "TimerQueue.h"               // InsertTimer
#include "StreamDeps.h"               // G_CEnvironment()
#include "CEnvironment.h"             // get_channel_no

// ORIG G_TimerQueue W @0x080f647c（定义 TimerQueue.cpp；无权威头声明，此处 extern）
TimerQueue* G_TimerQueue();

namespace ARAD {

#include "Arad_ServerStateManager/notifyOpenMessageDialog.inc"
                             const AradServerStateMessage& msg);

// ---------------------------------------------------------------------------
// SSendEffect —— sendState 内部 for_each 函子。
//   C1 0x081a9dac（存 user）；D1 0x081a9dba（空体）；
//   operator()(const pair&) 0x081a9dc0：
//     notifyOpenMessageDialog(m_user, 1, (int)pair.first, pair.second)。
// ---------------------------------------------------------------------------
class SSendEffect
{
public:
    explicit SSendEffect(CUser* user) : m_user(user) {}   // ORIG 0x081a9dac
    ~SSendEffect() {}                                     // ORIG 0x081a9dba

    void operator()(const std::pair<const ENUM_GLOBAL_EFFECT_ARAD,
                                    AradServerStateMessage>& p) const
    {
        // ORIG 0x081a9dc0（W 弱内联）：第二实参恒 1、第三实参为键。
        notifyOpenMessageDialog(m_user, 1, (int)p.first, p.second);
    }

private:
    CUser* m_user;  // +0x00（ctor 仅存指针，对象 4 字节）
};

#include "Arad_ServerStateManager/Arad_ServerStateManager.inc"


#include "Arad_ServerStateManager/dtor_Arad_ServerStateManager.inc"


#include "Arad_ServerStateManager/insertState.inc"


#include "Arad_ServerStateManager/insertState_1.inc"


#include "Arad_ServerStateManager/insertStateNoSync.inc"


#include "Arad_ServerStateManager/findState.inc"


#include "Arad_ServerStateManager/deleteState.inc"


#include "Arad_ServerStateManager/sendState.inc"


#include "Arad_ServerStateManager/checkContinus.inc"


#include "Arad_ServerStateManager/attachTimer.inc"


#include "Arad_ServerStateManager/loadServerState.inc"


#include "Arad_ServerStateManager/saveServerState.inc"


#include "Arad_ServerStateManager/sendEffectPacket.inc"


}  // namespace ARAD
