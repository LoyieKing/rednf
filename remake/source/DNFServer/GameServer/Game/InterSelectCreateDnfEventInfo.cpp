// df_game_r 还原 —— InterSelectCreateDnfEventInfo（台湾内部事件分发器）。
// 依据报告：
//   makeRequest  docs/class_func_reports/InterSelectCreateDnfEventInfo/makeRequest.md（0x8160ef6）
//   dispatch_sig docs/class_func_reports/InterSelectCreateDnfEventInfo/dispatch_sig.md（0x8160d5e）
//   Reward       docs/class_func_reports/InterSelectCreateDnfEventInfo__Reward/（0x8160c4a / 0x8160c6e）
//   C1/D1/D0     objdump 0x816201a/0x816203e/0x816206e（无报告，nm 定址转写）
// 依赖：TaiwanInterfaceInterDispatcher（基类）实现由 OrigTaiwanDisp 落；
// CEventCreateDnf / CEventManager / MsgQueueMgr / StreamPool 已有权威实现。
#include "InterSelectCreateDnfEventInfo.h"

#include "CInventory.h"      // PacketGuard 完整定义（含 InterfacePacketBuf 基础方法）
#include "CUser.h"
#include "CStreamGuard.h"
#include "TaiwanInternal.h"  // Taiwan::internal_stream
#include "GlobalData.h"      // s_stream_pool / s_msgq_mgr / s_event_manager
#include "MsgQueueMgr.h"
#include "CGuildServerProxy.h"  // CEventManager（GetRepeatEvent）
#include "CEventCreateDnf.h"
#include "GameWorld.h"       // ProcessAllUsers 模板 + G_GameWorld()
#include "LogManager.h"

// ---- Reward ----
// ORIG 0x8160c4a：+0x00 short（rate），+0x02 char（flag）。
InterSelectCreateDnfEventInfo::Reward::Reward(int rate, bool flag)
{
    m_rate = (unsigned short)rate;
    m_flag = flag;
}

// ORIG 0x8160c6e：组包 0x22d，rate + 奖励条件字节，finalize 后发送。
// Ghidra C：put_header(0,0x22d); put_short(m_rate);
//   if (GetEventCreateDnfReward() == 1 || m_flag == 0) put_byte(0) else put_byte(1);
//   finalize(true); user->Send(guard);
void InterSelectCreateDnfEventInfo::Reward::operator()(CUser* user)
{
    PacketGuard guard;
    guard.put_header(0, 0x22d);
    guard.put_short(m_rate);
    // [b28d-56 判词·AE] ORIG 0x8160cb4 段逐条：`call; xor $1,%eax; test %al,%al;
    // je L0`（P248 档① bool callee + 裸 `!`）→ `movzbl 0x2(%eax); test %al,%al;
    // je L0`（m_flag 直测 !=0）→ `mov $1; jmp L1 / L0: mov $0; L1: test %al;
    // je put0; put1; jmp; put0:`（P214① 正条件 `&&` 物化，put1 直落、put0 后置）。
    // 依赖：CUser::GetEventCreateDnfReward char→bool（Main 已代落，P292）。
    // 探针 /tmp/lane56/sc.cpp g1 逐条复现。
    if (!user->GetEventCreateDnfReward() && m_flag != 0)
    {
        guard.put_byte(1);
    }
    else
    {
        guard.put_byte(0);
    }
    guard.finalize(true);
    user->Send(guard);
}

// ---- C1 / D1 / D0 ----
// ORIG 0x816201a：call 基类 ctor(code=9) + 设 vtable 0x8b7aca8。
InterSelectCreateDnfEventInfo::InterSelectCreateDnfEventInfo()
    : TaiwanInterfaceInterDispatcher(static_cast<TaiwanInternalPack::T>(9))
{
}

// ORIG 0x816203e：设 vptr + 调基类 D1 + 尾部 delete 检查（D1 无 delete）。
InterSelectCreateDnfEventInfo::~InterSelectCreateDnfEventInfo()
{
}

// ORIG 0x816206e：call D1 + operator delete。
// （编译器生成 D0，无需手写。）

// ---- makeRequest ----
// ORIG 0x8160ef6：Acquire stream → CStreamGuard(stream,true) →
// internal_stream(guard, 9, 0xffffffff) → guard->GetInBuffer<Msg>()；
// 写 hour/rate/eventDay 三 int 入 MsgQueueMgr::put(1, guard)。
void InterSelectCreateDnfEventInfo::makeRequest(int rate, int hour, int eventDay)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp", 0x11b),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(9), 0xffffffff);
    Msg* msg = guard->GetInBuffer<Msg>();
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x120,
                              "[Taiwan, Event] hour:%d rate:%d event_day:%d",
                              hour, rate, eventDay);
    }
    else
    {
        msg->m_rate = rate;
        msg->m_hour = hour;
        msg->m_eventDay = eventDay;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
}

// ---- dispatch_sig ----
// ORIG 0x8160d5e。第二参 data 实际承载 Msg*（ORIG 汇编 0x10(%ebp)=data 判空；
// Ghidra C 的 param_3 标注为反编译器 thiscall 参数索引偏差，以汇编为准）。
// 流程：
//   data==0 → 日志 "interMsg is null."，返回 0xfe；
//   event = GetRepeatEvent(0xa0)；
//   if (event==0 || vtable+0x34 槽(event,0) != 1) 视为「事件未进行」：
//       再调槽取字节 + 日志 stayEvent=ok/nullptr eventing:%d，返回 0x108；
//   否则：hour<0 时 hour=GetRate()&0xffff；SetEventInfo(rate,hour,eventDay)；
//       Reward(GetRate()&0xffff, IsRewardCondition()) 经 ProcessAllUsers 分发，返回 0。
// 注：vtable+0x34 槽为 CEventBase 判活直调（与 Dispatchers.cpp BingoQuiz
// check_error 同口径；CGuildServerProxy.h 槽位注释与 ORIG 槽序有出入，用直调保 ABI）。
int InterSelectCreateDnfEventInfo::dispatch_sig(CUser* user, char* data, int len)
{
    // [b28d-56 判词·N] N7/P282：vcall 偏移已核（两处 vptr+0x34 = _ZTV+0x3c =
    // word[15]，P48 无 CSE 各自独立调用，P167 CLEAR 0 missing/0 extra）；残余为
    // 4.4.7 间接 call 目标恒 %eax vs ORIG %edx（N7，4.4.6 后端可翻，本 TU 未路由）
    // + 槽号平移 4（msg -0x14 vs -0x10）。真语义 bug 已修（m_rate/m_hour）。
    Msg* msg = reinterpret_cast<Msg*>(data);
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0xfd,
                              "[Taiwan, Event] interMsg is null.");
        return 0xfe;
    }
    CEventCreateDnf* event =
        (CEventCreateDnf*)GlobalData::s_event_manager->GetRepeatEvent(0xa0);
    typedef bool (*IsEventingFn)(void*, void*);
    // [b28d-56 判词·AE] ORIG 0x08160dbf 段逐条：`cmpl $0,event; je L1` +
    // `call *vptr+0x34; xor $1; test; je L2`（P214① 正条件 `||` 物化：L1 mov $1 /
    // L2 mov $0 → test）；未进行分支第二次虚调用**内联在 logFormat 实参位**
    // （P44/P92：ORIG 0x08160e0e `call *%edx; movzbl %al,%edx` 后直接压栈，
    // 无 fn/ev 具名局部）。第二次调用的 vtable 偏移同为 +0x34（P282：vptr+0x34
    // = _ZTV+0x3c = word[15]；两处独立调用，P48 无 CSE）。
    if (event == 0 ||
        !reinterpret_cast<IsEventingFn>(
            (*reinterpret_cast<void***>(event))[0x34 / 4])(event, 0))
    {
        // P0：ORIG 无 stay 具名槽（`mov $"nullptr",%eax; jmp; mov $"ok",%eax`
        // 直接物化进 %eax 后压 0x14(%esp)）⇒ 三目内联在实参位。
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
            __PRETTY_FUNCTION__, 0x107,
            "[Taiwan, Event] stayEvent=%s eventing:%d",
            (event == 0) ? "nullptr" : "ok",
            (int)(unsigned char)reinterpret_cast<IsEventingFn>(
                (*reinterpret_cast<void***>(event))[0x34 / 4])(event, 0));
        return 0x108;
    }
    // [b28d-56 真语义 bug 修正] ORIG 0x08160e63 判/写的是 msg+0x00 = m_rate
    // （makeRequest 0x08160fb2 `msg+0 = param_1(rate)` 实证），旧码误用 m_hour
    // （msg+0x04）。SetEventInfo 实参序 (msg+4, msg+0, msg+8) = (m_hour, m_rate,
    // m_eventDay) 与现调用一致。
    if ((int)msg->m_rate < 0)
    {
        msg->m_rate = (int)((unsigned short)event->GetRate());
    }
    event->SetEventInfo(msg->m_hour, msg->m_rate, msg->m_eventDay);
    // P0：ORIG 无 cond/rate 具名槽（IsRewardCondition 结果留 %ebx、GetRate 直传），
    // 故内联为 ctor 实参。
    Reward reward((int)(unsigned short)event->GetRate(), event->IsRewardCondition());
    G_GameWorld()->ProcessAllUsers<InterSelectCreateDnfEventInfo::Reward>(reward);
    return 0;
}

// CEventCreateDnf::GetState 定义置于本 TU（与 CEventCreateDnf.cpp 中
// IsRewardCondition 调用者分离）：4.4 unit-at-a-time 对 TU 内可见 callee
// 塌缩 outgoing 帧（sub $0x4），ORIG 4.1 恒发 sub $0x18（探针 p6/p7 同族）。
int CEventCreateDnf::GetState()
{
    return static_cast<int>(m_state);
}
