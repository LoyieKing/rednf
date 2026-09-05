// df_game_r 还原 —— TaiwanInternalDispatcher（台湾内部分发器，ORIG T @0x081601d0 族）。
// 报告：docs/class_func_reports/TaiwanInternalDispatcher/（6 函数齐全）。
// 依赖：init 中 new 的 10 个子类（InterLoadServiceRestrictPolicy 等）为 ORIG W ctor；
// InterSelectCreateDnfEventInfo 的 ctor 由 OrigScriptInfo 批次实现（本 TU 仅 U 引用），
// 其余 9 个子类 ctor/dtor 最小实现（仅基类 ctor + vptr，证据：global_constructors_
// keyed_to_TaiwanInternalDispatcher 报告各 ctor 区 movl code + call 基类 C1 + 设子类 vtable）。
#include "TaiwanInternalDispatcher.h"

#include <algorithm>

#include "TaiwanInterfaceInterDispatcher.h"
#include "TaiwanInternal.h"      // Taiwan::internal_stream（makeRequest 依赖）

#include "InterLoadServiceRestrictPolicy.h"
#include "InterLoadGeolocation.h"
#include "InterCheckStayTimeEvent.h"
#include "InterSelectCreateDnfEventInfo.h"
#include "InterSelectCreateDnfUserInfo.h"
#include "InterSelectMobileAuthReward.h"
#include "InterSelectPcroomDailyReward.h"
#include "InterUpdateP2pHolePunchingState.h"
#include "InterInsertGiveBoxEventReward.h"
#include "InterLoadHeroMissionEventData.h"
// OnlyOrigBatch4：InterSelectCreateDnfUserInfo::makeRequest/dispatch_sig 依赖。
#include "GlobalData.h"        // s_stream_pool / s_msgq_mgr / s_event_manager
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "LogManager.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "CUser.h"
#include "CEventCreateDnf.h"
// OnlyOrigBatch14：InterSelectMobileAuthReward/InterUpdateP2pHolePunchingState
// dispatch_sig 依赖。
#include "CEventCeraShopRewardPoint.h"
#include "CMailBoxHelper.h"
#include "Inven_Item.h"
#include "RDARScriptStringManager.h"
#include "RestrictGeolocation.h"
#include "CStatisticServerProxy.h"
#include "CServerProxyMgr.h"
#include "CEnvironment.h"
#include "../../../shared/packet/include/Packet_GameServer2Statisctics2DBServer.h"

// ---- Inter* 子类最小实现（仅基类 ctor + vptr；业务方法不在本批）----

InterLoadServiceRestrictPolicy::InterLoadServiceRestrictPolicy() throw()
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)1)  // ORIG @0x08161ecd movl $0x1
{
}
InterLoadServiceRestrictPolicy::~InterLoadServiceRestrictPolicy()
{
}

InterLoadGeolocation::InterLoadGeolocation() throw()
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)4)  // ORIG @0x08161f3f movl $0x4
{
}
InterLoadGeolocation::~InterLoadGeolocation()
{
}

InterCheckStayTimeEvent::InterCheckStayTimeEvent() throw()
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)5)  // ORIG @0x08161fb1 movl $0x5
{
}
InterCheckStayTimeEvent::~InterCheckStayTimeEvent()
{
}

InterSelectCreateDnfUserInfo::InterSelectCreateDnfUserInfo() throw()
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)0xa)  // ORIG @0x08162095 movl $0xa
{
}
// ORIG W 0x081620b0/0x081620e0：vptr + 基类析构（D0 另加 operator delete）。
InterSelectCreateDnfUserInfo::~InterSelectCreateDnfUserInfo()
{
}
// ---- makeRequest（InterSelectMobileAuthReward）----
// ORIG T 0x081615ae：同 CreateDnfUserInfo 模式，internal_stream(guard, 0xc,
// param_1)；空 → logFormat(0x18b)；否则写 +0x0/+0x4 入 Msg 并 put(1, guard)。
void InterSelectMobileAuthReward::makeRequest(int param_1, unsigned int param_2,
                                              bool param_3)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp", 0x186),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0xc), param_1);
    Msg* msg = guard->GetInBuffer<Msg>();
    if (msg == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x18b, "[Taiwan, MobileAuth] m_id:%d rewarded:%d", param_2,
            (unsigned int)param_3);
    }
    else
    {
        msg->m_id = param_2;
        msg->m_rewarded = param_3 != 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
}

// ---- makeRequest ----
// ORIG T 0x08161218：Acquire stream → CStreamGuard(stream,true) →
// internal_stream(guard, 0xa, uid) → guard->GetInBuffer<Msg>()；
// 写 m_id/received 入 MsgQueueMgr::put(1, guard)。
void InterSelectCreateDnfUserInfo::makeRequest(int uid, unsigned int memberId,
                                               bool received)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp", 0x14d),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0xa), uid);
    Msg* msg = guard->GetInBuffer<Msg>();
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x152,
                              "[Taiwan, Event] m_id:%d isReceived:%d",
                              memberId, received);
    }
    else
    {
        msg->m_id = memberId;
        msg->m_received = received;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
}

// ---- dispatch_sig ----
// ORIG T 0x08161012。第二参 buf 实际承载 Msg*（判空后读 +0x0/+0x4）。
// 流程：buf==0 → 日志 "interMsg is null." 返回 0x12f；
//   event = GetRepeatEvent(0xa0)；
//   if (event==0 || vtable+0x34 槽(event,0) != 1) 视为「事件未进行」：
//       再调槽取字节 + 日志 stayEvent=ok/nullptr eventing:%d，返回 0x139；
//   否则：SetEventCreateDnfReward(buf+4)；发 0x22d 包（rate + rewardable byte），返回 0。
// 注：vtable+0x34 槽为 CEventBase 判活直调（与 InterSelectCreateDnfEventInfo 同口径，
//   见其 dispatch_sig 注释；ORIG 槽序与 CGuildServerProxy.h 注释有出入，用直调保 ABI）。
int InterSelectCreateDnfUserInfo::dispatch_sig(CUser* user, char* buf, int len)
{
    if (buf == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x12e,
                              "[Taiwan, Event] interMsg is null.");
        return 0x12f;
    }
    CEventCreateDnf* event =
        (CEventCreateDnf*)GlobalData::s_event_manager->GetRepeatEvent(0xa0);
    typedef bool (*IsEventingFn)(void*, void*);
    bool eventing;
    if (event == 0 ||
        reinterpret_cast<IsEventingFn>(
            (*reinterpret_cast<void***>(event))[0x34 / 4])(event, 0) != '\x01')
    {
        eventing = true;
    }
    else
    {
        eventing = false;
    }
    if (eventing)
    {
        IsEventingFn fn = reinterpret_cast<IsEventingFn>(
            (*reinterpret_cast<void***>(event))[0x34 / 4]);
        char ev = fn(event, 0);
        const char* stay = (event == 0) ? "nullptr" : "ok";
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x138,
                              "[Taiwan, Event] stayEvent=%s eventing:%d",
                              stay, ev & 0xff);
        return 0x139;
    }
    bool rewardable = false;
    if (buf[4] != '\x01' && event->IsRewardCondition())
    {
        rewardable = true;
    }
    user->SetEventCreateDnfReward(buf[4] != 0);
    PacketGuard pg;
    pg.put_header(0, 0x22d);
    pg.put_short((unsigned short)event->GetRate());
    pg.put_byte((int)rewardable);
    pg.finalize(true);
    user->Send(pg);
    return 0;
}

InterSelectMobileAuthReward::InterSelectMobileAuthReward() throw()
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)0xc)  // ORIG @0x08162107 movl $0xc
{
}
InterSelectMobileAuthReward::~InterSelectMobileAuthReward()
{
}

InterSelectPcroomDailyReward::InterSelectPcroomDailyReward() throw()
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)0xd)  // ORIG @0x08162179 movl $0xd
{
}
InterSelectPcroomDailyReward::~InterSelectPcroomDailyReward()
{
}

InterUpdateP2pHolePunchingState::InterUpdateP2pHolePunchingState() throw()
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)0xf)  // ORIG @0x081621eb movl $0xf
{
}
InterUpdateP2pHolePunchingState::~InterUpdateP2pHolePunchingState()
{
}

InterInsertGiveBoxEventReward::InterInsertGiveBoxEventReward() throw()
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)0x10)  // ORIG @0x0816225d movl $0x10
{
}
InterInsertGiveBoxEventReward::~InterInsertGiveBoxEventReward()
{
}

InterLoadHeroMissionEventData::InterLoadHeroMissionEventData() throw()
    : TaiwanInterfaceInterDispatcher((TaiwanInternalPack::T)0x12)  // ORIG @0x081622cf movl $0x12
{
}
InterLoadHeroMissionEventData::~InterLoadHeroMissionEventData()
{
}

// ---- TaiwanInterfaceInterDispatcher 基类（ORIG W：C1 0x08161e32 / D1 0x08161e4a / D0 0x08161e70 / getCode 0x08161e8e）----

TaiwanInterfaceInterDispatcher::TaiwanInterfaceInterDispatcher(TaiwanInternalPack::T code) throw()
    : m_code(code)
{
}
TaiwanInterfaceInterDispatcher::~TaiwanInterfaceInterDispatcher()
{
}
TaiwanInternalPack::T TaiwanInterfaceInterDispatcher::getCode() const
{
    return m_code;
}

// ---- TaiwanInternalDispatcher ----

TaiwanInternalDispatcher::TaiwanInternalDispatcher()
{
    m_dispatchers.clear();   // ORIG @0x081601d0：map::map + map::clear
}

TaiwanInternalDispatcher::~TaiwanInternalDispatcher()
{
    std::for_each(m_dispatchers.begin(), m_dispatchers.end(), FnDelete());  // ORIG @0x08160212
}

void TaiwanInternalDispatcher::FnDelete::operator()(
    std::pair<const TaiwanInternalPack::T, TaiwanInterfaceInterDispatcher*>& item)
{
    delete item.second;   // ORIG @0x08161e9a：test+je + vtable D0 调用
}

TaiwanInterfaceInterDispatcher* TaiwanInternalDispatcher::getDispatcher(TaiwanInternalPack::T code)
{
    std::map<TaiwanInternalPack::T, TaiwanInterfaceInterDispatcher*>::iterator it =
        m_dispatchers.find(code);
    if (it != m_dispatchers.end())
    {
        return it->second;
    }
    return 0;
}

bool TaiwanInternalDispatcher::addDispatcher(TaiwanInternalPack::T code,
                                             TaiwanInterfaceInterDispatcher* disp)
{
    if (getDispatcher(code) != 0)
    {
        return false;
    }
    m_dispatchers[code] = disp;
    return true;
}

void TaiwanInternalDispatcher::delDispatcher(TaiwanInternalPack::T code)
{
    if (getDispatcher(code) != 0)
    {
        m_dispatchers.erase(m_dispatchers.find(code));
    }
}

bool TaiwanInternalDispatcher::init()
{
    TaiwanInterfaceInterDispatcher* p = 0;
    p = new InterLoadServiceRestrictPolicy();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterLoadGeolocation();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterCheckStayTimeEvent();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterSelectCreateDnfEventInfo();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterSelectCreateDnfUserInfo();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterSelectMobileAuthReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterSelectPcroomDailyReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterUpdateP2pHolePunchingState();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterInsertGiveBoxEventReward();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    p = new InterLoadHeroMissionEventData();
    if (p != 0 && addDispatcher(p->getCode(), p) != 1)
    {
        return false;
    }

    return true;
}

// ============================================================================
// OnlyOrigBatch14：InterSelectMobileAuthReward::dispatch_sig
// ORIG T 0x0816132a（0x283）。buf 承载 Msg*。
// 注意：ORIG 0x08161384 处为无条件 jmp（E9 04 02 00 00 → +0x263），
// 发邮件主体（make_pair/findString/ReqDBSendNewSystemMultiMail/SendReward）
// 在 ORIG 中为不可达代码；执行路径仅回写 user+0x8ec31 = m_rewarded。
//
// 【N84 归因（batch-11 实测）】we-smaller 143 行 = ORIG 4.1 -O0 对
// `if ((msg->rewarded ^ 1) != 0 && <const-false>)` 型条件保留首项求值
// （0x08161378..58 movzbl/xor $1/test）+ 无条件 jmp + **文本发射不可达 body**
// （+0x79..+0x262：vector pair/findString×2/strncpy×2/9-loop/
// MakeSystemMultiMailPostal/ReqDBSendNewSystemMultiMail/GetRepeatEvent/SendReward
// + EH vector dtor cleanup）。gcc 4.4 -O0 对同形条件整体折叠（/tmp/ehtest.C
// 探针：`if(msg && (r^1) && 0)` 仅输出 tail+mov$0，首项求值与 body 全消）；
// volatile 守卫探针（ehtest2.C）可出 body 但条件三段 short-circuit 形距 ORIG
// `test;jmp` 更远、且需新增 TU static 符号（链接污染）+误启用发信行为风险。
// ⇒ 编译期不可构造，按 blocked；本函数保持「仅可达路径」语义忠实实现。
// ============================================================================
int InterSelectMobileAuthReward::dispatch_sig(CUser* user, char* buf, int len)
{
    Msg* msg = reinterpret_cast<Msg*>(buf);
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__,
                              0x161, "[Taiwan, MobileAuth] interMsg is null.");
        return 0x162;
    }
    user->m_field8ec31 = msg->m_rewarded;
    return 0;
}

// ============================================================================
// OnlyOrigBatch14：InterUpdateP2pHolePunchingState::dispatch_sig
// ORIG T 0x08161952：P2P 洞穿状态包经统计代理转发（含国家代码）。
// ============================================================================
// 消息体读侧视图（§4.3）：ORIG 单段 disp `movzbl 0xd(%eax)`/`mov 0xe(%eax)`/
// `mov 0x12(%eax)`（0x081619xx）；宽=装载指令。
#pragma pack(push, 1)
struct P2pHoleMsgV
{
    char m_pad0[0xd];           // +0x00..未消费
    unsigned char m_connectedType;  // +0x0d
    int m_requiredTime;         // +0x0e
    int m_checkTime;            // +0x12
};
#pragma pack(pop)

int InterUpdateP2pHolePunchingState::dispatch_sig(CUser* user, char* buf, int len)
{
    // ORIG 0x08161952：统计包 → 国家代码 + 对端 IP → 经统计代理 UDP 转发。
    // ORIG 0x081619xx：buf 入口单槽物化 -0xc（无 char*+view 双中转，p 删除），
    // 三处字段读均经该槽 reload（技巧36 反向）。
    P2pHoleMsgV* v = reinterpret_cast<P2pHoleMsgV*>(buf);
    Packet_GameServer2Statisctics2DBServer pkt;
    pkt.m_serverGroup = (short)G_CEnvironment()->get_server_group();
    // ORIG `movzbl 0xd(%eax)`/`mov 0xe(%eax)`/`mov 0x12(%eax)` 单段 disp ⇒
    // pack(1) 视图具名字段（§4.3；裸 add 形实测不折 disp）。
    pkt.m_connectedType = v->m_connectedType;
    pkt.m_requiredTime = v->m_requiredTime;
    pkt.m_checkTime = v->m_checkTime;
    user->m_network.GetPeerIP2(pkt.m_peerAddress, 0x10);
    RestrictGeolocation::Get()->getCountryCode(
        pkt.m_nationCode, 0x10, std::string(pkt.m_peerAddress));
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)0);
    proxy->SendPacket((char*)&pkt, 0x35);
    return 0;
}

// ============================================================================
// OnlyOrigBatch13 组 C —— 5 个 Inter dispatch_sig（ORIG 逐函数对照）
// ============================================================================
// df_game_r 还原 —— Inter 类 dispatch_sig 批次 C（5 个台湾域 Inter 分发函数）。
// 报告：docs/class_func_reports/{InterCheckStayTimeEvent,InterInsertGiveBoxEventReward,
//   InterLoadGeolocation,InterLoadHeroMissionEventData,InterLoadServiceRestrictPolicy}/dispatch_sig.md
// 语义逐指令对齐 ORIG（返回码 / 日志行号 / 调用序照抄反汇编）。

#include "TaiwanInterfaceInterDispatcher.h"
#include "CUser.h"                    // CUser / GetServerGroup / getCurCharacNo / IsFirstTimeLogin
#include "GlobalData.h"               // GlobalData::s_event_manager
#include "CGuildServerProxy.h"        // CEventManager::GetRepeatEvent / CEventBase
#include "CDataManager.h"             // G_CDataManager / find_item
#include "CItem.h"                    // CItem::make_item / get_index
#include "Inven_Item.h"               // Inven_Item::set_add_info
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "EventGiveMeBox.h"           // EventGiveMeBox::kRewardItem_
#include "CEventStayTime.h"           // CEventStayTime::kRewardItem_（ORIG 类静态）
#include "CEventStayTime.h"           // CEventStayTime::kRewardItem_（ORIG 类静态）
#include "RestrictGeolocation.h"      // RestrictGeolocation::addAllowCountry（含 ARAD::Singleton）
#include "ServiceRestrictManager.h"   // ServiceRestrictManager::beginPolicy / RestrictCategory
#include "HeroMissionEvent.h"         // HeroMissionEvent::setEventData/processMission/sendMissionData
#include "GameTypes.h"                // cMyTrace

#include <cstring>   // strlen
#include <string>

// ---- 载荷视图（pack(1)，按报告/反汇编真实布局；禁止裸偏移）----
#pragma pack(push, 1)
// InterLoadGeolocation：+0x00 countryCount(int)，+0x04 起每 16 字节一个 country code C 串
// （上限 0x2d 个，ORIG 反汇编 shl $0x4 步进实证）。
struct SigLoadGeolocation
{
    int countryCount;              // +0x00
    char countryCode[0x2d][16];    // +0x04
};

// InterLoadHeroMissionEventData：+0x00 头 4 字节，+0x04 起事件数据串（setEventData(buf+4)）。
struct SigLoadHeroMissionEventData
{
    char header[4];                // +0x00
    char data[1];                  // +0x04（C 字符串）
};

// InterLoadServiceRestrictPolicy：+0x00 category(int)，+0x119d 起策略串
// （ORIG 反汇编 lea 0x119d(%eax) 实证）。
struct SigLoadServiceRestrictPolicy
{
    int category;                  // +0x00
    char pad[0x119d - 0x04];       // +0x04 .. +0x119c
    char policyString[1];          // +0x119d（C 字符串）
};
#pragma pack(pop)

// ===== InterCheckStayTimeEvent::dispatch_sig —— ORIG 0x08160a42 =====
// buf==0 → cMyTrace(0xc9) 日志 "[Taiwan, StayEvent] SigStayTimeEvent is null." 返回 0xca；
// GetRepeatEvent(0x9f) 经 vtable+0x34 槽（ORIG IsEventing）判活，!=1 返回 0；
// 奖励道具 CEventStayTime::kRewardItem_E（0x2920908d）find_item 失败 → 日志(0xd3) 返回 0；
// 否则 make_item 填 Inven_Item、set_add_info(1)，取标题/正文脚本串，经
// ReqDBSendNewSystemMail(…, 0xe, group, false, false) 发系统邮件，返回 0。
int InterCheckStayTimeEvent::dispatch_sig(CUser* user, char* buf, int len)
{
    if (buf == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0xc9, 5);
        tr("[Taiwan, StayEvent] SigStayTimeEvent is null.");
        return 0xca;
    }
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x9f);
    // ORIG 直调 vtable+0x34 槽（同 TaiwanInternalDispatcher.cpp 既有口径，保 ABI；无空检）。
    typedef bool (*IsEventingFn)(void*, void*);
    if (reinterpret_cast<IsEventingFn>(
            (*reinterpret_cast<void***>(event))[0x34 / 4])(event, 0) != '\x01')
    {
        return 0;
    }
    CItem* item = G_CDataManager()->find_item(CEventStayTime::kRewardItem_);
    if (item == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0xd3, 5);
        tr("[Taiwan, StayEvent] Invalid item index. (%u)", CEventStayTime::kRewardItem_);
        return 0;
    }
    Inven_Item inv;
    item->get_index();            // ORIG 调用但丢弃返回值，保留调用序
    item->make_item(inv);
    inv.set_add_info(1);
    const char* title = g_scriptStringManager_.findString(4, "tw_ccb_stress_title_01", 0);
    const char* mail = g_scriptStringManager_.findString(4, "tw_ccb_stress_mail_01", 0);
    ENUM_SERVER_GROUP group = user->GetServerGroup();
    unsigned int characNo = (unsigned int)user->getCurCharacNo();
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, inv, 0, characNo, mail, (int)strlen(mail), 0xe, group, false, false);
    return 0;
}

// ===== InterInsertGiveBoxEventReward::dispatch_sig —— ORIG 0x08161a8a =====
// 同 InterCheckStayTimeEvent 模式：event 0xa5、道具 EventGiveMeBox::kRewardItem_、
// 脚本串 tw_givemebox2_title/tw_givemebox2_mail；buf==0 → 日志(0x1dc) 返回 0x1dd。
int InterInsertGiveBoxEventReward::dispatch_sig(CUser* user, char* buf, int len)
{
    if (buf == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1dc, 5);
        tr("[Taiwan, GiveMeBoxEvent] msg is null.");
        return 0x1dd;
    }
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0xa5);
    typedef bool (*IsEventingFn)(void*, void*);
    if (reinterpret_cast<IsEventingFn>(
            (*reinterpret_cast<void***>(event))[0x34 / 4])(event, 0) != '\x01')
    {
        return 0;
    }
    CItem* item = G_CDataManager()->find_item(EventGiveMeBox::kRewardItem_);
    if (item == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1e6, 5);
        tr("[Taiwan, GiveMeBoxEvent] Invalid item index. (%u)",
           EventGiveMeBox::kRewardItem_);
        return 0;
    }
    Inven_Item inv;
    item->get_index();            // ORIG 调用但丢弃返回值，保留调用序
    item->make_item(inv);
    inv.set_add_info(1);
    const char* title = g_scriptStringManager_.findString(4, "tw_givemebox2_title", 0);
    const char* mail = g_scriptStringManager_.findString(4, "tw_givemebox2_mail", 0);
    ENUM_SERVER_GROUP group = user->GetServerGroup();
    unsigned int characNo = (unsigned int)user->getCurCharacNo();
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, inv, 0, characNo, mail, (int)strlen(mail), 0xe, group, false, false);
    return 0;
}

// ===== InterLoadGeolocation::dispatch_sig —— ORIG 0x0816088a =====
// buf==0 → cMyTrace(0xac) 日志 "[Taiwan, GeoIP] SigLoadRestrictPolicy is null." 返回 0xad；
// countryCount>0x2d → 日志(0xb2) 并写回 0x2d；循环 addAllowCountry 每国码（16 字节块），
// 每项 cMyTrace(0xb9,9,true,true) 日志 "GeoIP Allow Country Code : %s"；返回 0。
int InterLoadGeolocation::dispatch_sig(CUser* user, char* buf, int len)
{
    if (buf == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0xac, 5);
        tr("[Taiwan, GeoIP] SigLoadRestrictPolicy is null.");
        return 0xad;
    }
    SigLoadGeolocation* sig = reinterpret_cast<SigLoadGeolocation*>(buf);
    if (sig->countryCount > 0x2d)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0xb2, 5);
        tr("[Taiwan, GeoIP] Invalid GeoIP country count. (%d)", sig->countryCount);
        sig->countryCount = 0x2d;
    }
    for (int i = 0; i < sig->countryCount; ++i)
    {
        // ORIG：构造临时 string 直接值传（无拷贝），addAllowCountry 参数槽复用
        ARAD::Singleton<RestrictGeolocation>::Get()->addAllowCountry(
            std::string(sig->countryCode[i]));
        cMyTrace tr(__PRETTY_FUNCTION__, 0xb9, 9, true, true);
        tr("GeoIP Allow Country Code : %s", sig->countryCode[i]);
    }
    return 0;
}

// ===== InterLoadHeroMissionEventData::dispatch_sig —— ORIG 0x08161c92 =====
// buf==0 → cMyTrace(0x1ff) 日志 "[HeroMission] msg is null." 返回 0x200；
// setEventData(user, buf+4)；IsFirstTimeLogin 命中 → processMission(user, 5, 0)；
// sendMissionData(user)；返回 0。
int InterLoadHeroMissionEventData::dispatch_sig(CUser* user, char* buf, int len)
{
    if (buf == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1ff, 5);
        tr("[HeroMission] msg is null.");
        return 0x200;
    }
    SigLoadHeroMissionEventData* sig =
        reinterpret_cast<SigLoadHeroMissionEventData*>(buf);
    HeroMissionEvent* hm = ARAD::Singleton<HeroMissionEvent>::Get();
    hm->setEventData(user, sig->data);
    if (user->IsFirstTimeLogin())
    {
        hm = ARAD::Singleton<HeroMissionEvent>::Get();
        hm->processMission(user, HeroMissionCondition::MissionNo::T_5, 0);
    }
    hm = ARAD::Singleton<HeroMissionEvent>::Get();
    hm->sendMissionData(user);
    return 0;
}

// ===== InterLoadServiceRestrictPolicy::dispatch_sig —— ORIG 0x08160796 =====
// buf==0 → cMyTrace(0x94) 日志 "[Taiwan, Restrict] SigLoadRestrictPolicy is null." 返回 0x95；
// string 取 buf+0x119d，beginPolicy(Get(), *(int*)buf, string)；返回 0。
int InterLoadServiceRestrictPolicy::dispatch_sig(CUser* user, char* buf, int len)
{
    if (buf == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x94, 5);
        tr("[Taiwan, Restrict] SigLoadRestrictPolicy is null.");
        return 0x95;
    }
    SigLoadServiceRestrictPolicy* sig =
        reinterpret_cast<SigLoadServiceRestrictPolicy*>(buf);
    std::string policyStr(sig->policyString);
    ARAD::Singleton<ServiceRestrictManager>::Get()->beginPolicy(
        (RestrictCategory::Enum)sig->category, policyStr);
    return 0;
}
