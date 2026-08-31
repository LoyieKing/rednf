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
int notifyOpenMessageDialog(CUser* user, int a, int b,
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

// ---------------------------------------------------------------------------
// ctor —— ORIG 0x081a922c：*this 置 0（m_flag=0）、Mutex 构造、map 构造。
// ---------------------------------------------------------------------------
Arad_ServerStateManager::Arad_ServerStateManager()
    : m_flag(0), m_lock(), m_stateMap()
{
}

// ---------------------------------------------------------------------------
// dtor —— ORIG 0x081a927e：map 析构、Mutex 析构（与成员声明逆序一致）。
// ---------------------------------------------------------------------------
Arad_ServerStateManager::~Arad_ServerStateManager()
{
}

// ---------------------------------------------------------------------------
// insertState(arad::SigServerState*) —— ORIG 0x081a92ca：
// Guard 内遍历 sig->count 条目，逐条 insertStateNoSync(tag.code, tag.msg)。
// ORIG 键读取自 sig + i*20 + 0x9 == tagData.m_code；消息位于 sig+i*20+0xd == tagData.m_msg。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::insertState(arad::SigServerState* sig)
{
    Guard<Mutex> guard(&m_lock);
    for (int i = 0; i < sig->m_count; ++i)
    {
        insertStateNoSync((ENUM_GLOBAL_EFFECT_ARAD)sig->m_data[i].m_code,
                          sig->m_data[i].m_msg);
    }
}

// ---------------------------------------------------------------------------
// insertState(ENUM_GLOBAL_EFFECT_ARAD, AradServerStateMessage const&)
// —— ORIG 0x081a936e：Guard 内直接 insertStateNoSync。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::insertState(ENUM_GLOBAL_EFFECT_ARAD code,
                                          const AradServerStateMessage& msg)
{
    Guard<Mutex> guard(&m_lock);
    insertStateNoSync(code, msg);
}

// ---------------------------------------------------------------------------
// insertStateNoSync —— ORIG 0x081a991a：
// 若 findState 未命中：map[code] = msg；
// saveServerState((1), code, msg, true)；sendEffectPacket(code, msg, 0)；
// cMyTrace("[SERVER_STAT] apply server stat. ...")（endSec 取自 msg+2 域 dword）。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::insertStateNoSync(
    ENUM_GLOBAL_EFFECT_ARAD code, const AradServerStateMessage& msg)
{
    AradServerStateMessage found;
    if (findState(code, found))
        return;

    m_stateMap[code] = msg;
    saveServerState(K_NOTIFY_EVENT_CATEGORY_SERVER_STAT, code, msg, true);
    sendEffectPacket(code, msg, 0);
    cMyTrace(__PRETTY_FUNCTION__, 0x88, 0)(
        "[SERVER_STAT] apply server stat. (category:%d, code:%d, end:%u)", 1,
        (int)code, msg.m_endSec);
}

// ---------------------------------------------------------------------------
// findState —— ORIG 0x081a9c2a（非 const 方法）：
// map::find 命中 → out = it->second 返回 true；否则 memset(out,0,0xc) 返回 false。
// ---------------------------------------------------------------------------
bool Arad_ServerStateManager::findState(ENUM_GLOBAL_EFFECT_ARAD code,
                                        AradServerStateMessage& out)
{
    std::map<ENUM_GLOBAL_EFFECT_ARAD, AradServerStateMessage>::iterator it =
        m_stateMap.find(code);
    if (it != m_stateMap.end())
    {
        out = it->second;
        return true;
    }
    memset(&out, 0, sizeof(out));
    return false;
}

// ---------------------------------------------------------------------------
// deleteState —— ORIG 0x081a93d4：
// Guard 内 findState；命中则置空态消息（word@+0=0 等价 active 归零，
// 其余字段保持 ctor 全零初值）、sendEffectPacket(code, cleared, 0)、
// saveServerState(1, code, cleared, false)、trace、map::erase(code)。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::deleteState(ENUM_GLOBAL_EFFECT_ARAD code)
{
    Guard<Mutex> guard(&m_lock);
    AradServerStateMessage cleared;
    if (findState(code, cleared))
    {
        // ORIG：movw $0,(local)——仅将起始 word 清零；局部消息其余字段为 ctor 全零。
        cleared.m_active = 0;

        sendEffectPacket(code, cleared, 0);
        saveServerState(K_NOTIFY_EVENT_CATEGORY_SERVER_STAT, code, cleared, false);
        cMyTrace(__PRETTY_FUNCTION__, 0x38, 0)(
            "[SERVER_STAT] delete server stat. (category:%d, code:%d, end:%u)", 1,
            (int)code, cleared.m_endSec);
        m_stateMap.erase(code);
    }
}

// ---------------------------------------------------------------------------
// sendState(CUser*) —— ORIG 0x081a9500：
// Guard 内以 SSendEffect 遍历全 map，逐条 notifyOpenMessageDialog(user,1,code,msg)。
// ORIG 用 std::for_each 驱动（弱实例 _ZSt8for_each…SSendEffect @0x81a9ee5）。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::sendState(CUser* user)
{
    Guard<Mutex> guard(&m_lock);
    std::for_each(m_stateMap.begin(), m_stateMap.end(), SSendEffect(user));
}

// ---------------------------------------------------------------------------
// checkContinus —— ORIG 0x081a95dc：
// Guard 内取 curSec；遍历 map：active==1 且 curSec>=endSec 的条目 →
// 置 active=0、sendEffectPacket(0)、saveServerState(…, false)、trace、erase(it++)；
// 循环自然结束后 attachTimer() 重挂定时器。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::checkContinus()
{
    Guard<Mutex> guard(&m_lock);
    const unsigned long curSec = GlobalData::s_systemTime_.getCurSec();

    std::map<ENUM_GLOBAL_EFFECT_ARAD, AradServerStateMessage>::iterator it =
        m_stateMap.begin();
    while (it != m_stateMap.end())
    {
        if (curSec >= it->second.m_endSec && it->second.m_active == 1)
        {
            it->second.m_active = 0;
            sendEffectPacket(it->first, it->second, 0);
            saveServerState(K_NOTIFY_EVENT_CATEGORY_SERVER_STAT, it->first,
                            it->second, false);
            cMyTrace(__PRETTY_FUNCTION__, 0x56, 0)(
                "[SERVER_STAT] delete server stat. (category:%d, code:%d, end:%u)",
                1, (int)it->first, it->second.m_endSec);
            m_stateMap.erase(it++);
        }
        else
        {
            ++it;
        }
    }
    attachTimer();
}

// ---------------------------------------------------------------------------
// attachTimer —— ORIG 0x081a98d4：
// G_TimerQueue()->InsertTimer(OBJ_TYPE 2, 0, TIMER_MESSAGE 0xac, sec 0x78, 0, 0)。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::attachTimer()
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0xac, 0x78, 0, 0);
}

// ---------------------------------------------------------------------------
// loadServerState —— ORIG 0x081a97f8：
// SigServerState 初值（流内 GetInBuffer 反序列化用），从 StreamPool 取流 →
// make_internal_stream_jpn(guard, K_LOAD_SERVER_STATE(0x29), -1) →
// put_binary(sig, 0x195) → MsgQueueMgr::put(DB_Q(2))。
// ORIG 走 guard.operator->() 链（0x80c8c4e）再 put_binary（P103）。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::loadServerState()
{
    arad::SigServerState sig;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localjapan/Arad_ServerStateManager.cpp", 0x6a),
                       true);
    DISPATCHER::make_internal_stream_jpn(guard, K_LOAD_SERVER_STATE, -1);
    guard.operator->()->put_binary(&sig, 0x195);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---------------------------------------------------------------------------
// saveServerState —— ORIG 0x081a9afa：
// 渠道号==10 时：SigServerState::set(endFlag, category, code, msg) →
// Acquire(line 0xb2) → make_internal_stream_jpn(guard, K_SAVE_SERVER_STATE(0x2a),
// -1) → put_binary(sig, 0x195) → MsgQueueMgr::put(DB_Q)。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::saveServerState(ENUM_NOTIFY_EVENT_CATEGORY category,
                                              ENUM_GLOBAL_EFFECT_ARAD code,
                                              const AradServerStateMessage& msg,
                                              bool endFlag)
{
    if (G_CEnvironment()->get_channel_no() != 10)
        return;

    arad::SigServerState sig;
    sig.set(endFlag, (int)category, (int)code, msg);
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localjapan/Arad_ServerStateManager.cpp", 0xb2),
                       true);
    DISPATCHER::make_internal_stream_jpn(guard, K_SAVE_SERVER_STATE, -1);
    guard.operator->()->put_binary(&sig, 0x195);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---------------------------------------------------------------------------
// sendEffectPacket —— ORIG 0x081a9a1c：
// PacketGuard 组包：put_header(0,0x21a)/put_int(1)/put_int(code)/
// put_binary(msg,0xc)/finalize(true)；user==NULL → G_GameWorld()->send_all，
// 否则 user->Send。
// ---------------------------------------------------------------------------
void Arad_ServerStateManager::sendEffectPacket(ENUM_GLOBAL_EFFECT_ARAD code,
                                               const AradServerStateMessage& msg,
                                               CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 0x21a);
    packet.put_int(1);
    packet.put_int((int)code);
    packet.put_binary((char*)&msg, 0xc);   // ORIG put_binary(char*,int)（非 const 重载）
    packet.finalize(true);

    if (user == 0)
        G_GameWorld()->send_all(packet);
    else
        user->Send(packet);
}

}  // namespace ARAD
