// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Arad_ServerStateManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// notifyOpenMessageDialog 权威实现归 ARAD.cpp（ORIG 0x81978dd）。
// b28d-Main：与权威头 Arad_DataManager.h:142 同步为 bool（S59 P248 档①，ORIG 返回 bool）。
#include "Arad_ServerStateManager/notifyOpenMessageDialog.inc"
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

// ---------------------------------------------------------------------------
// ctor —— ORIG 0x081a922c：*this 置 0（m_flag=0）、Mutex 构造、map 构造。
// ---------------------------------------------------------------------------
#include "Arad_ServerStateManager/Arad_ServerStateManager.inc"
}

// ---------------------------------------------------------------------------
// dtor —— ORIG 0x081a927e：map 析构、Mutex 析构（与成员声明逆序一致）。
// ---------------------------------------------------------------------------
#include "Arad_ServerStateManager/dtor_Arad_ServerStateManager.inc"
}

// ---------------------------------------------------------------------------
// insertState(arad::SigServerState*) —— ORIG 0x081a92ca：
// Guard 内遍历 sig->count 条目，逐条 insertStateNoSync(tag.code, tag.msg)。
// ORIG 键读取自 sig + i*20 + 0x9 == tagData.m_code；消息位于 sig+i*20+0xd == tagData.m_msg。
// ---------------------------------------------------------------------------
#include "Arad_ServerStateManager/insertState.inc"
}

// ---------------------------------------------------------------------------
// insertState(ENUM_GLOBAL_EFFECT_ARAD, AradServerStateMessage const&)
// —— ORIG 0x081a936e：Guard 内直接 insertStateNoSync。
// ---------------------------------------------------------------------------
#include "Arad_ServerStateManager/insertState_1.inc"
}

// ---------------------------------------------------------------------------
// insertStateNoSync —— ORIG 0x081a991a：
// 若 findState 未命中：map[code] = msg；
// saveServerState((1), code, msg, true)；sendEffectPacket(code, msg, 0)；
// cMyTrace("[SERVER_STAT] apply server stat. ...")（endSec 取自 msg+2 域 dword）。
// ---------------------------------------------------------------------------
#include "Arad_ServerStateManager/insertStateNoSync.inc"
}

// ---------------------------------------------------------------------------
// findState —— ORIG 0x081a9c2a（非 const 方法）：
// map::find 命中 → out = it->second 返回 true；否则 memset(out,0,0xc) 返回 false。
// ---------------------------------------------------------------------------
#include "Arad_ServerStateManager/findState.inc"
}

// ---------------------------------------------------------------------------
// deleteState —— ORIG 0x081a93d4：
// Guard 内 findState；命中则置空态消息（word@+0=0 等价 active 归零，
// 其余字段保持 ctor 全零初值）、sendEffectPacket(code, cleared, 0)、
// saveServerState(1, code, cleared, false)、trace、map::erase(code)。
// ---------------------------------------------------------------------------
#include "Arad_ServerStateManager/deleteState.inc"
}

// ---------------------------------------------------------------------------
// sendState(CUser*) —— ORIG 0x081a9500：
// Guard 内以 SSendEffect 遍历全 map，逐条 notifyOpenMessageDialog(user,1,code,msg)。
// ORIG 用 std::for_each 驱动（弱实例 _ZSt8for_each…SSendEffect @0x81a9ee5）。
// ---------------------------------------------------------------------------
#include "Arad_ServerStateManager/sendState.inc"
}

// ---------------------------------------------------------------------------
// checkContinus —— ORIG 0x081a95dc：
// Guard 内取 curSec；遍历 map：active==1 且 curSec>=endSec 的条目 →
// 置 active=0、sendEffectPacket(0)、saveServerState(…, false)、trace、erase(it++)；
// 循环自然结束后 attachTimer() 重挂定时器。
// ---------------------------------------------------------------------------
#include "Arad_ServerStateManager/checkContinus.inc"
}

// ---------------------------------------------------------------------------
// attachTimer —— ORIG 0x081a98d4：
// G_TimerQueue()->InsertTimer(OBJ_TYPE 2, 0, TIMER_MESSAGE 0xac, sec 0x78, 0, 0)。
// ---------------------------------------------------------------------------
#include "Arad_ServerStateManager/attachTimer.inc"
}

// ---------------------------------------------------------------------------
// loadServerState —— ORIG 0x081a97f8：
// SigServerState 初值（流内 GetInBuffer 反序列化用），从 StreamPool 取流 →
// make_internal_stream_jpn(guard, K_LOAD_SERVER_STATE(0x29), -1) →
// put_binary(sig, 0x195) → MsgQueueMgr::put(DB_Q(2))。
// ORIG 走 guard.operator->() 链（0x80c8c4e）再 put_binary（P103）。
// ---------------------------------------------------------------------------
#include "Arad_ServerStateManager/loadServerState.inc"
}

// ---------------------------------------------------------------------------
// saveServerState —— ORIG 0x081a9afa：
// 渠道号==10 时：SigServerState::set(endFlag, category, code, msg) →
// Acquire(line 0xb2) → make_internal_stream_jpn(guard, K_SAVE_SERVER_STATE(0x2a),
// -1) → put_binary(sig, 0x195) → MsgQueueMgr::put(DB_Q)。
// ---------------------------------------------------------------------------
#include "Arad_ServerStateManager/saveServerState.inc"
}

// ---------------------------------------------------------------------------
// sendEffectPacket —— ORIG 0x081a9a1c：
// PacketGuard 组包：put_header(0,0x21a)/put_int(1)/put_int(code)/
// put_binary(msg,0xc)/finalize(true)；user==NULL → G_GameWorld()->send_all，
// 否则 user->Send。
// ---------------------------------------------------------------------------
#include "Arad_ServerStateManager/sendEffectPacket.inc"
}

}  // namespace ARAD
