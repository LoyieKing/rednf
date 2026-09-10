// ============================================================================
// ExchangeServer_Exchange.cpp —— exchange_server::CExchangeServer 全方法
// OnlyOrig_Exchange.ExExchangeTU 批（Wave5）。
// 逐指令转写自 ORIG 反汇编快照 /tmp/es_disasm.txt：
//   段 _ZN15exchange_server15CExchangeServer* @0x8491004..0x8493a7e +
//   ExchangeServerInfo C1 @0x8491004。
// 权威布局以只读头 ExchangeServer.h（#9F8A 修订版）为准；vtable 实证：
//   CExchangeServer vtable 0x8c7a1d0 = [DispatchPacket, UnregisterExchangeServer,
//   AllocSession, FreeSession, GetServerType]（5 槽，无虚析构）。
// ============================================================================
#include "ExchangeServer.h"

#include <arpa/inet.h>
#include <time.h>

#include "CacheCharacterType.h"
#include "CMonitorServerProxy.h"      // SendExchangeServerInfo(int,int,short)
#include "CServerProxyMgr.h"
#include "CStreamGuard.h"
#include "GlobalData.h"               // s_msgq_mgr / s_*_stream_pool / s_monitor_proxy_mgr
#include "InterfacePacketBuf.h"
#include "MsgQueueMgr.h"
#include "StaticPool.h"

namespace exchange_server
{

// ---------------------------------------------------------------------------
// 便捷名（ORIG 直访全局 0x940bd68/0x940bd6c/0x940bda4/0x940bda8/0x940be28）
// ---------------------------------------------------------------------------
typedef MsgQueueMgr::QUEUE_IDX QIdx;

// ---------------------------------------------------------------------------
// OnRecvMessageServerInfo 线格式视图（ORIG 0x8493220 起 `mov 0xa(%eax)` /
// `movzwl 0xe(%eax)` / `mov 0x10(%eax)` 逐字段重读同一具名指针）——§4.3 具名化，
// 非对齐偏移 0x0a/0x0e ⇒ packed。
// ---------------------------------------------------------------------------
struct ExchangeServerInfoMsgView
{
    char m_pad00[0x0a];
    int m_channel0a;
    unsigned short m_port0e;
    unsigned int m_ip10;
} __attribute__((packed));

// ---------------------------------------------------------------------------
// ExchangeServerInfo::ExchangeServerInfo   ORIG 0x8491004（逐域清零）
// ---------------------------------------------------------------------------
ExchangeServerInfo::ExchangeServerInfo()
    : m_field00(0), m_word04(0), m_type08(EXCHANGE_CONNECT_UNKNOWN),
      m_pSession(NULL)
{
}

// ---------------------------------------------------------------------------
// GetServerType / SetServerType            ORIG 0x8491030 / 0x849103c
// ---------------------------------------------------------------------------
EXCHANGE_SERVER_TYPE CExchangeServer::GetServerType()
{
    return (EXCHANGE_SERVER_TYPE)m_serverType68;                    // +0x68
}

void CExchangeServer::SetServerType(EXCHANGE_SERVER_TYPE type)
{
    // 枚举字段=枚举形参同型直赋（无转换节点）：4.4 LHS 先载=ORIG 实证。
    m_serverType68 = type;
}

// ---------------------------------------------------------------------------
// AllocSession / FreeSession               ORIG 0x849104a / 0x8491060
// （StaticPool<CSession,300>@+0x6c）
// ---------------------------------------------------------------------------
CSession* CExchangeServer::AllocSession()
{
    return m_sessionPool.Acquire();
}

void CExchangeServer::FreeSession(CSession* session)
{
    m_sessionPool.Free(session);
}

// ---------------------------------------------------------------------------
// Reset                                    ORIG 0x849107e
// ---------------------------------------------------------------------------
void CExchangeServer::Reset()
{
    m_flag38 = false;
    m_flag39 = false;
    // N92 翻案（b28 L11）：中转仅见于 long→unsigned int 字段；字段改 time_t 同型(long)
    // 后直存形 `mov 0x8(%ebp),%edx; mov %eax,0x3c(%edx)` 与 ORIG 全等（探针 l11_n92）。
    m_resetTime3c = time(NULL);
}

// ---------------------------------------------------------------------------
// GetNetworkEvent                          ORIG 0x84910a6
// （WaitForSessionEvent(10ms)；返回值忽略，恒 true）
// ---------------------------------------------------------------------------
bool CExchangeServer::GetNetworkEvent()
{
    m_sessionMgr.WaitForSessionEvent(10);
    return true;
}

// ---------------------------------------------------------------------------
// GetExchangeServerCount                   ORIG 0x84910ca
// （遍历 map 比较 value.m_type08 == type）
// ---------------------------------------------------------------------------
int CExchangeServer::GetExchangeServerCount(CONNECT_TYPE type)
{
    int count = 0;
    // P212 头部声明 + P13 缓存 end：ORIG 头段 begin→-0x10、end→-0x14 各求值一次，
    // 4.4 头部声明先声明浅 ⇒ it=-0x10/e=-0x14 精确命中（旧 for-init + 条件内
    // 重调 end() 致循环体多 5 条重评线）。
    std::map<int, ExchangeServerInfo>::iterator it = m_serverMap.begin();
    std::map<int, ExchangeServerInfo>::iterator end = m_serverMap.end();
    for (; it != end; ++it)
    {
        if (it->second.m_type08 == type)
        {
            ++count;
        }
    }
    return count;
}

// ---------------------------------------------------------------------------
// SetMaster                                ORIG 0x849114c
// ---------------------------------------------------------------------------
void CExchangeServer::SetMaster()
{
    cMyTrace trace(
        __PRETTY_FUNCTION__, 0xf6, 0);
    trace("SetMaster()");
    SetServerType(EXCHANGE_SERVER_MASTER);
    m_flag38 = true;
}

// ---------------------------------------------------------------------------
// SetSlave                                 ORIG 0x84911a4
// ---------------------------------------------------------------------------
void CExchangeServer::SetSlave()
{
    cMyTrace trace(
        __PRETTY_FUNCTION__, 0xfe, 0);
    trace("SetSlave()");
    SetServerType(EXCHANGE_SERVER_SLAVE);
    m_flag38 = false;
}

// ---------------------------------------------------------------------------
// TrySend                                  ORIG 0x84911fc
// （map[serverId].m_pSession 存在则经 SessionManager 转发）
// ---------------------------------------------------------------------------
bool CExchangeServer::TrySend(int serverId, PacketGuard& guard)
{
    std::map<int, ExchangeServerInfo>::iterator it =
        m_serverMap.find(serverId);
    // P168a③b 实测定稿（探针 /tmp/l11_x.cpp f7）：合并 && 使 4.4 出 mov$1/mov$0
    // 岛；**嵌套 if** 形下第二条件出 ORIG 的 `test; setne %al; test; je <共享FAIL尾>`
    // 物化形，bool callee(ne) 第一条件直 `test %al; je` 与 ORIG 全等。
    if (it != m_serverMap.end())
    {
        if (it->second.m_pSession != 0)
        {
            return m_sessionMgr.TrySend(it->second.m_pSession, guard);
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// AttachSessionInExchangeServer            ORIG 0x8491296
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：P221/P227 临时链复现 serverId 跨 ctor 保活 + trace 槽 -0x18。
bool CExchangeServer::AttachSessionInExchangeServer(int serverId,
                                                    CSession* session)
{
    // [b28d-48 复核] 维持 N93附+P75（NDIFF 53，53/55 条）：iterator 对槽深
    // 方向 + session 形参跨 cMyTrace ctor 保活差。
    // [b28c-6] fresh 复验维持 N93附+P75（ndiff 22，55/53 条）：
    // [b28-L11 判 N·N93附+P75] 残差两类：①函数级 iterator 对(-0x20/-0x1c)与
    // if 尾块内 cMyTrace(-0x18) 的槽深方向跨版本相反（N93 附：兄弟/块内对象
    // 分配方向，P212 头部声明法对带参 ctor 类不可用）；②ORIG push %ebx 将
    // session 形参跨 cMyTrace ctor 调用保活（`mov %ebx,0x8(%esp)`），4.4 恒在
    // 组参点重读形参槽（P75/N82附 多实参首值寄存器对调）——ndiff 22 封顶。
    session->SetChannelNo(serverId);
    std::map<int, ExchangeServerInfo>::iterator it =
        m_serverMap.find(serverId);
    if (it != m_serverMap.end())
    {
        it->second.m_pSession = session;
        return true;
    }
    // P221/P227 临时链：复现 ORIG 0x8491314 `mov 0xc(%ebp),%ebx` + trace 槽 -0x18。
    cMyTrace("bool exchange_server::CExchangeServer::"
             "AttachSessionInExchangeServer(int, "
             "exchange_server::CSession*)", 0x11b, 0)
        ("AttachSessionInExchangeServer() channel_no(%d) failed", serverId);
    return false;
}

// ---------------------------------------------------------------------------
// RegisterExchangeServer                   ORIG 0x849135c
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：P293 具名 it（find 先/end 后）+ P221 临时链保活。
bool CExchangeServer::RegisterExchangeServer(int serverId,
                                             ExchangeServerInfo& info)
{
    // [b28d-solo-Exchange] P293：ORIG 0x849137a 先 find（it=-0x50）后 end
    // （临时 -0x4c）⇒ 具名 it 形（单表达式 find()==end() 在 4.4 反产 end 先）。
    std::map<int, ExchangeServerInfo>::iterator it =
        m_serverMap.find(serverId);
    if (it == m_serverMap.end())
    {
        m_serverMap.insert(std::make_pair(serverId, info));
        return true;
    }
    // P221/P227 临时链：复现 ORIG 0x8491404 `mov 0xc(%ebp),%ebx` + trace 槽 -0x18。
    cMyTrace("bool exchange_server::CExchangeServer::"
             "RegisterExchangeServer(int, "
             "exchange_server::ExchangeServerInfo&)", 0x128, 0)
        ("RegisterExchangeServer() channel_no(%d) failed", serverId);
    return false;
}

// ---------------------------------------------------------------------------
// UnregisterExchangeServer                 ORIG 0x849144c
// （移除后若无 SLAVE（CONNECT_TYPE=1）剩留则自动转 Master）
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：返回类型 void→bool（ORIG 成功 mov $1/失败 mov $0）+ P221 临时链。
bool CExchangeServer::UnregisterExchangeServer(int serverId)
{
    // [b28d-solo-Exchange] 返回类型 void→bool：ORIG 成功 `mov $0x1,%eax` /
    // 失败 `mov $0x0,%eax`（0x84914dc/0x8491520），虚函数返回类型不入 mangling。
    std::map<int, ExchangeServerInfo>::iterator it =
        m_serverMap.find(serverId);
    if (it != m_serverMap.end())
    {
        m_serverMap.erase(it);
        // ORIG 直传立即数 1 作 CONNECT_TYPE
        if (GetExchangeServerCount((CONNECT_TYPE)1) == 0)
        {
            SetMaster();
        }
        return true;
    }
    // P221/P227 临时链：复现 ORIG 0x84914e3 `mov 0xc(%ebp),%ebx`（serverId 跨
    // ctor 保活）与 trace 槽 -0x18（4.4 具名形恒落深槽 -0x20）。
    cMyTrace("virtual bool exchange_server::CExchangeServer::"
             "UnregisterExchangeServer(int)", 0x13b, 0)
        ("UnregisterExchangeServer() channel_no(%d) failed", serverId);
    return false;
}

// ---------------------------------------------------------------------------
// SendRequestCharacInfo                    ORIG 0x849152a
// 包：header(1,0x84) + a,b,c,d + extraLen + binary(extra)
// 发往 serverId = 第三个整参（与 ORIG 栈槽一致）
// ---------------------------------------------------------------------------
bool CExchangeServer::SendRequestCharacInfo(int a, int b, int serverId,
                                            int d, void* extra,
                                            int extraLen)
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x84);
    guard.put_int(a);
    guard.put_int(b);
    guard.put_int(serverId);
    guard.put_int(d);
    guard.put_int(extraLen);                       // ORIG: put_int(len) 后紧跟 binary
    guard.put_binary((char*)extra, extraLen);
    guard.finalize(false);
    if (TrySend(serverId, guard))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// ---------------------------------------------------------------------------
// SendResponseCharacInfo（带 session 版）  ORIG 0x8491644
// 包：header(1,0x85) + f + a + b + len + binary(data)
// ---------------------------------------------------------------------------
bool CExchangeServer::SendResponseCharacInfo(int a, int b, void* data,
                                             int len, CSession* session,
                                             int f)
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x85);
    guard.put_int(f);
    guard.put_int(a);
    guard.put_int(b);
    guard.put_int(len);
    guard.put_binary((char*)data, len);
    guard.finalize(false);
    return m_sessionMgr.TrySend(session, guard);
}

// ---------------------------------------------------------------------------
// SendResponseCharacInfo（无 session 版）  ORIG 0x849173e → _forchina
// ---------------------------------------------------------------------------
bool CExchangeServer::SendResponseCharacInfo(int a, int b, void* data,
                                             int len)
{
    // P190 撑帧（同 ForwardRequestCharac）：双 double 取址 pad 使 4.4 帧 0x28→0x38。
    double framePad1, framePad2;
    (void)&framePad1;
    (void)&framePad2;
    return SendResponseCharacInfo_forchina(a, b, data, len);
}

// ---------------------------------------------------------------------------
// SendResponseCharacInfo_forchina          ORIG 0x849176e
// 三档池取流（total=len+8：<=0xfff Small / <=0x3fff Default / else Big），
// 写 a、b、binary(data) 后投递 MsgQueueMgr idx=1（DISPATCH_Q）。
// 注：ORIG 返回值取 MsgQueueMgr::put 尾部 eax（`mov %eax,%ebx` 整字直传，非规范化），
// 本函数返回 put 的结果（权威头 MsgQueueMgr::put 声明 bool，转发 Stream* 版返回值）。
// 判词：else if 写成 (total > 0xfff && total <= 0x3fff) 复现 ORIG 第二分支入口的
// 冗余重测 `cmpl $0xfff; jle`（4.1 不消除短路链中已知条件，4.4 复用前次比较结果
// 需显式保留 && 前项）。check_tu 实测 identical 口径。
// ---------------------------------------------------------------------------
bool CExchangeServer::SendResponseCharacInfo_forchina(int a, int b,
                                                      void* data, int len)
{
    CStreamGuard sGuard(true);
    int total = len + 8;
    if (total <= 0xfff)
    {
        sGuard.Set(GlobalData::s_small_stream_pool->Acquire("ExchangeServer.cpp",
                                                0x17a));
    }
    else if (total > 0xfff && total <= 0x3fff)
    {
        sGuard.Set(GlobalData::s_stream_pool->Acquire("ExchangeServer.cpp", 0x17e));
    }
    else
    {
        sGuard.Set(GlobalData::s_big_stream_pool->Acquire("ExchangeServer.cpp",
                                              0x182));
    }

    // ORIG 显式走 CStreamGuard::operator*()（deEv，返回 this 指针）再在其上调
    // operator<<(int)（lsEi）；put_binary 走 operator->()（ptEv）——P103 链不可省略。
    sGuard.operator*()->operator<<(a);
    sGuard.operator*()->operator<<(b);
    sGuard.operator->()->put_binary(data, len);

    // ORIG mov %eax,%ebx：直接返回 MsgQueueMgr::put 的结果（非硬编码 true）。
    return GlobalData::s_msgq_mgr->put(QIdx(1), sGuard);
}

// ---------------------------------------------------------------------------
// ReturnCacheCharacter                     ORIG 0x84918d8
// 本 channel 命中缓存并回包；参数 e 控制命中后是否归还池内存。
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] 126/127（p167 CLEAR）：P231 纯 register bool 复现 ORIG 的
// `&&` 短路物化岛（P226 语句表达式对内存第二条件被 4.4 jump-thread，探针 p1-t2；
// register 形探针 p7-t47 出岛，仅多 1 条 `mov %eax,%ebx`，ORIG 值在 %al）。
// 真 bug 已修：GetCacheCharacter 实参 c→d。
// 残差=块内局部槽向（pCache -0x34 vs -0x14、trace -0x30/-0x20 vs -0x2c/-0x3c，
// N93 附）+ 1 条寄存器中转 mov 封顶。
bool CExchangeServer::ReturnCacheCharacter(int a, int b, int c, int d,
                                           bool e, CSession* session)
{
    // [b28d-solo-Exchange] ORIG 0x8491926 `test %al; je <else>` 正极性 ⇒
    // `if (GetCacheCharacter(...)) { 成功 } else { trace 0x1d6 }`（旧码取反写、
    // 且具名 mgr 多槽，P0 应内联）。P226 语句表达式复现 valid 的零槽短路物化
    // （ORIG 0x8491963-0x849197f，valid 无槽往返）。
    bool handled = false;
    if (GetServerChannelNo() == c)
    {
        CACHE_CHARACTER_TYPE* pCache = NULL;

        // [b28d-solo-Exchange 真 bug] ORIG 0x8491917 压 `0x18(%ebp)` = 第 4 形参
        // d（charac_no），旧码误传 c（channel_no）；比较 `== c` 才是 channel_no。
        if (GetInstanceCacheCharacterMgr()->GetCacheCharacter(d, &pCache, e))
        {
            int pos = 0;
            int memberIdx = 0;
            bool found = false;
            pos = pCache->GetMemberPosition((ENUM_INTERNALPACKET)a,
                                            &memberIdx, found);
            // [b28e] P226 语句表达式对「内存字段」第二条件被 4.4 jump-thread（探针
            // /tmp/probe_es p1-t2）；ORIG 0x8491963..0x849197f 为 && 结果物化岛
            // （mov$1/jmp/mov$0/test）且无槽 ⇒ P231 纯 register 存储类提示复现
            // （探针 p7-t47：岛形一致，仅 test 寄存器 %bl vs ORIG %al）。
            register bool valid = (pos != 0 && found);
            if (valid)
            {
                if (session != NULL)
                {
                    SendResponseCharacInfo(a, b, (void*)pos, memberIdx,
                                           session, 0);
                }
                else
                {
                    SendResponseCharacInfo(a, b, (void*)pos, memberIdx);
                }
                handled = true;
            }
            else
            {
                // [b28e 本泳道] P221/P227 全表达式临时链：ORIG 0x84919ec/0x8491a3e
                // 两处 trace 分别占 -0x30/-0x20（浅于 pCache -0x34）；具名对象形在
                // 4.4 取反方向（-0x2c/-0x3c）⇒ 改构造即调用临时链。
                cMyTrace("bool exchange_server::CExchangeServer::"
                         "ReturnCacheCharacter(int, int, int, int, bool, "
                         "exchange_server::CSession*)", 0x1bf, 0)(
                    "GetMemberPosition() invalid information  type(%d), "
                    "idx(%d), channel_no(%d), charac_no(%d)", a, b, c, d);
            }
            if (e)
            {
                GetInstanceCacheCharacterMgr()->FreeCharacter(pCache);
            }
        }
        else
        {
            cMyTrace("bool exchange_server::CExchangeServer::"
                     "ReturnCacheCharacter(int, int, int, int, bool, "
                     "exchange_server::CSession*)", 0x1d6, 0)(
                "GetCacheCharacter() false  type(%d), idx(%d), "
                "channel_no(%d), charac_no(%d)", a, b, c, d);
        }
    }
    return handled;
}

// ---------------------------------------------------------------------------
// ForwardRequestCharac_forchina            ORIG 0x8491ab6
// 同 _forchina 但：入参 len>0x20 时先计 CacheLoadFailCount；
// 投递队列改为 idx=2（DB_Q）。
// ---------------------------------------------------------------------------
bool CExchangeServer::ForwardRequestCharac_forchina(int a, int b,
                                                    void* data, int len)
{
    // [b24ab] ORIG `cmpl $0x20,0xc(%ebp)` 检查首参 a（this@8,a@c,b@10,data@14,
    // len@18），非 len（len@18 供 total=len+8）。原 `if(len==0x20)` 语义 bug。
    if (a == 0x20)
    {
        GetInstanceExchangeServerHitRate()->IncrementCacheLoadFailCount();
    }

    CStreamGuard sGuard(true);
    int total = len + 8;
    if (total <= 0xfff)
    {
        sGuard.Set(GlobalData::s_small_stream_pool->Acquire("ExchangeServer.cpp",
                                                0x1ec));
    }
    else if (total > 0xfff && total <= 0x3fff)
    {
        sGuard.Set(GlobalData::s_stream_pool->Acquire("ExchangeServer.cpp", 0x1f0));
    }
    else
    {
        sGuard.Set(GlobalData::s_big_stream_pool->Acquire("ExchangeServer.cpp",
                                              0x1f4));
    }

    sGuard.operator*()->operator<<(a);       // deEv → op<<(int)
    sGuard.operator*()->operator<<(b);       // deEv → op<<(int)
    sGuard.operator->()->put_binary(data, len);  // ptEv → put_binary

    // [b24ab] ORIG `call put; mov %eax,%ebx; ... mov %ebx,%eax; ret` 直接返回
    // put 结果（非硬编码 true），与 _forchina 兄弟同构。
    return GlobalData::s_msgq_mgr->put(QIdx(2), sGuard);
}

// ---------------------------------------------------------------------------
// ForwardRequestCharac                     ORIG 0x8491c32 → _forchina
// ---------------------------------------------------------------------------
bool CExchangeServer::ForwardRequestCharac(int a, int b, void* data,
                                           int len)
{
    // P190 撑帧：ORIG(4.1) sub $0x38 vs 4.4 紧凑 $0x28；两个 double 局部（仅取址）
    // 触发 4.4 8B 对齐撑帧 +0x10 精确命中，outgoing %esp 槽位不受影响
    // （探针 /tmp/l11_pad.cpp f1=0x28/f2=0x38）。
    double framePad1, framePad2;
    (void)&framePad1;
    (void)&framePad2;
    return ForwardRequestCharac_forchina(a, b, data, len);
}

// ---------------------------------------------------------------------------
// OnRecvPacketCharacInfo                   ORIG 0x84922ee（返回失败行号/0）
// 包字段：type, idx, channel_no, charac_no[, msg_size, binary]
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] 199/200（p167 CLEAR）：P225 while 形补 setne 物化 + 去具名
// fromChannel（P0）+ trace 全表达式临时链（P227，复现 ORIG 的 4 对深槽求值/回读）。
// 残差=循环出口对齐 nop（N112）+ SendResponseCharacInfo 实参装载序 1 条 mov（P75）
// + CACHE_CHARACTER_TYPE 帧基 -0x3f8b5 vs -0x3f8bc（N82）封顶。
int CExchangeServer::OnRecvPacketCharacInfo(CSession* session,
                                            PacketGuard& guard)
{
    // [b28d-48] N82 封顶（NDIFF 155，194/199 条）：巨件帧/槽平移 +
    // PacketGuard 临时槽方向。
    int errBook = 0;                                  // -0x44 (类型 a)
    int idx = 0;                                      // -0x48
    int chNoArg = 0;                                  // -0x4c
    int characNo = 0;                                 // -0x50
    bool localHit = false;                            // -0x1d

    if (!guard.get_int(errBook))                      // fail line 0x235
    {
        return 0x235;
    }
    if (!guard.get_int(idx))
    {
        return 0x236;
    }
    if (!guard.get_int(chNoArg))
    {
        return 0x237;
    }
    if (!guard.get_int(characNo))
    {
        return 0x238;
    }

    // ORIG：大对象置于栈帧 -0x3f8b5（sizeof(CACHE_CHARACTER_TYPE)=0x3f85c）
    CACHE_CHARACTER_TYPE cc;
    int pos = 0;                                      // -0x1c
    int mi1 = 0;                                      // -0x54
    int mi2 = 0;                                      // -0x58
    bool found = false;                               // -0x59
    pos = cc.GetMemberPosition((ENUM_INTERNALPACKET)errBook, &mi1, found);

    // P225：ORIG 0x84923c9 `cmpl $0,-0x1c; setne %al; test %al,%al; je` ⇒
    // while(v!=0){body;break;} 精确复现 4.1 bool 四段物化（4.4 对 if 恒直跳）。
    while (pos != 0)
    {
        if (!guard.get_int(mi2))                      // line 0x241
        {
            return 0x241;
        }
        if (!guard.get_binary((char*)pos, mi2))       // line 0x24d
        {
            return 0x24d;
        }
        break;
    }

    // P0：ORIG 0x084923bb `call GetChannelNo; mov %eax,%ebx` 无槽往返 ⇒ 原码
    // 无具名 fromChannel 局部（旧码多发一槽并把帧撑大 0x20）。
    // P227：ORIG 0x084923bd..0x084923e4 在 ctor 前右起求值 7 个实参
    // （characNo/chNoArg/mi1/mi2 落深槽，idx/errBook 落 %edi/%esi，
    // fromChannel 落 %ebx）⇒ 全表达式临时链复现。
    cMyTrace("int exchange_server::CExchangeServer::"
             "OnRecvPacketCharacInfo(exchange_server::CSession*, "
             "PacketGuard&)", 0x253, 0)
        ("OnRecvPacketCharacInfo() type(%d), idx(%d), msg len(%d), "
         "member_size(%d), channel_no(%d), charac_no(%d), "
         "from_channel_no",
         errBook, idx, mi2, mi1, chNoArg, characNo,
         session->GetChannelNo());

    if (errBook == 0x20)
    {
        localHit = true;
    }

    if (!ReturnCacheCharacter(errBook, idx, chNoArg, characNo, localHit,
                              session))
    {
        if (!SendResponseCharacInfo(errBook, idx, (void*)pos, mi1,
                                    session, 0x15))
        {
            cMyTrace trace("int exchange_server::CExchangeServer::"
                           "OnRecvPacketCharacInfo(exchange_server::"
                           "CSession*, PacketGuard&)", 0x25d, 0);
            trace("SendResponseCharacInfo() error");
        }
    }
    return 0;                                         // 全部正常路径 ret0
}

// ---------------------------------------------------------------------------
// OnRecvPacketCharacInfoRet                ORIG 0x84925e8
// 包字段：ret, type, idx[, msg_size, binary]
// ret==0 且本地回包失败（或 ret!=0）→ ForwardRequestCharac 兜底
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] DIFF→NEAR（165/166，p167 CLEAR）：P225 while 形补 setne 物化
// + 去具名 channel（P0）+ trace 全表达式临时链（P227，复现 ctor 前深槽/寄存器求值）。
// 残差=N82/N93 槽号平移（ORIG -0x38..-0x40 vs 我方 -0x2c..-0x34）封顶。
int CExchangeServer::OnRecvPacketCharacInfoRet(CSession* session,
                                               PacketGuard& guard)
{
    // [P31/P330 本泳道 2026-09-09] ORIG 入口零初始化**序列**为
    // `-0x38, -0x3c, -0x24, -0x20, -0x40`（5 条 movl，见 0x084925f4..0x08492610），
    // 旧码只声明 3 个（⇒ 我方 3 条 movl，帧 0x3f8bc vs ORIG 0x3f8cc）。
    // 其中 -0x24 / -0x20 两条写**在整个函数里再无读取**（已核：ORIG 切片内
    // grep `-0x24(%ebp)`/`-0x20(%ebp)` 各只命中这条 movl）⇒ 原码确有两个
    // 之后未被使用的局部（P31 死写先例，同 SetPeriodCheckTimer 的 pad24）。
    // 声明顺序照 ORIG 初始化顺序排，两个死局部按 ORIG 位置夹在中间。
    int idx = 0;                                      // ORIG -0x38
    int fromCh = 0;                                   // ORIG -0x3c
    int pad24 = 0;                                    // ORIG -0x24（死写，P31）
    int pad20 = 0;                                    // ORIG -0x20（死写，P31）
    int ret = 0;                                      // ORIG -0x40

    if (!guard.get_int(ret))                          // line 0x26c
    {
        return 0x26c;
    }
    if (!guard.get_int(idx))                          // line 0x26d
    {
        return 0x26d;
    }
    if (!guard.get_int(fromCh))                       // line 0x26e
    {
        return 0x26e;
    }

    CACHE_CHARACTER_TYPE cc;
    int pos = 0;
    int mi1 = 0;
    int mi2 = 0;
    bool found = false;
    pos = cc.GetMemberPosition((ENUM_INTERNALPACKET)idx, &mi1, found);

    // P225：ORIG 0x0849271a `cmpl $0x0,-0x1c(%ebp); setne %al; test %al,%al;
    // je` ⇒ while(v!=0){body;break;} 精确复现 4.1 bool 四段物化（4.4 对 if 直跳）。
    while (pos != 0)
    {
        if (!guard.get_int(mi2))                      // line 0x27a
        {
            return 0x27a;
        }
        if (!guard.get_binary((char*)pos, mi2))       // line 0x282
        {
            return 0x282;
        }
        break;
    }

    // P0：ORIG 0x08492778 `call GetChannelNo; mov %eax,%ebx` 无槽往返 ⇒ 原码
    // 无具名 channel 局部（旧码 `const int channel` 多发一槽）。
    // P227：ORIG 在 ctor 前求值 mi1/mi2/fromCh 到深槽、idx/ret 到 %edi/%esi、
    // channel 到 %ebx（0x0849277d..0x084927a5）⇒ 全表达式临时链复现。
    cMyTrace("int exchange_server::CExchangeServer::"
             "OnRecvPacketCharacInfoRet(exchange_server::"
             "CSession*, PacketGuard&)", 0x28a, 0)
        ("OnRecvPacketCharacInfoRet() ret(%d) type(%d), idx(%d), "
         "msg len(%d), member_size(%d), from_channel(%d)",
         ret, idx, fromCh, mi2, mi1, session->GetChannelNo());

    if (ret == 0)
    {
        // P276：ORIG 0x84927e3 `je 84927ec` 与 ret!=0 汇入**同一** ForwardRequestCharac
        // 调用点（p167 旧形 EXTRA +1）⇒ 成功即 return 0，失败落共享尾。
        if (SendResponseCharacInfo(idx, fromCh, (void*)pos, mi1))
        {
            return 0;
        }
    }
    ForwardRequestCharac(idx, fromCh, (void*)pos, mi1);
    return 0;
}

// ---------------------------------------------------------------------------
// OnRecvPacketServerInfo                   ORIG 0x8492854
// 包字段：serverId, ip, port(short)；Master 回全表、Slave 回空注册响应包
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] 282/282（gap 0，p167 CLEAR）：P227 临时链 + 具名 ipv 补回
// push %edi/esi/ebx 与帧 0x9c；真 bug 修复=trace 实参序（见函数内注释）。
// 残差=N82/N98 槽号差（ORIG info -0x5c/pkt -0x6c/trace -0x48 vs 我方
// -0x3c/-0x64/-0x3c）封顶。
int CExchangeServer::OnRecvPacketServerInfo(CSession* session,
                                            PacketGuard& guard)
{
    // [P330 本泳道] ORIG 0x08492860 入口**无** serverId 的零初始化（直接
    // `lea -0x4c(%ebp); get_int`）⇒ 原码是不带初值的局部，旧 `= 0` 多发一条
    // movl 且把后续槽位整体推偏。
    ExchangeServerInfo info;                          // ORIG 槽 -0x5c
    int serverId;                                     // ORIG 槽 -0x4c（无初值）

    if (!guard.get_int(serverId))                     // line 0x29d
    {
        return 0x29d;
    }
    if (!guard.get_int(info.m_field00))               // line 0x29e
    {
        return 0x29e;
    }
    // P0：ORIG 无 port16 槽（0x84928a0 `lea -0x5c(%ebp); add $0x4,%edx` 直传
    // &info.m_word04）⇒ 直接以 m_word04 为 get_short 出参（InterfacePacketBuf
    // 仅有 short& 重载）。
    if (!guard.get_short(*(short*)&info.m_word04))    // line 0x29f
    {
        return 0x29f;
    }
    info.m_type08 = (CONNECT_TYPE)2;                  // ORIG 立即数 2

    // [b28e 真 bug] ORIG 0x849291e/0x8492922/0x8492926 压栈序 = serverId(0x8)、
    // ip(0xc)、port(0x10)，与格式串 "channel no(%d), server ip(%s), port(%d)" 对应
    // ⇒ 实参序应为 (serverId, ip, port)；旧码 (ip, serverId, port) 把 %d 喂指针、
    // %s 喂 int（可变参类型不匹配，日志错乱）。
    // P0：ORIG 0x84928de `mov -0x5c(%ebp),%eax; mov %eax,-0x60(%ebp)`（槽往返）
    // ⇒ 原码有具名 ipv 局部。P227：inet_ntoa 在 cMyTrace ctor 前求值并跨 ctor 保活。
    struct in_addr ipv;
    ipv.s_addr = info.m_field00;
    cMyTrace("int exchange_server::CExchangeServer::"
             "OnRecvPacketServerInfo(exchange_server::CSession*, "
             "PacketGuard&)", 0x2a5, 0)
        ("channel no(%d), server ip(%s), port(%d)", serverId, inet_ntoa(ipv),
         (int)(short)info.m_word04);

    // [b28d-48 P48/P282] ORIG 0x0849293d 与 0x08492ac5 两处独立
    // `mov (%eax),%eax; add $0x10,%eax; … call *%edx`（vptr+0x10 = GetServerType
    // 槽）：O0 无 CSE，双臂各一次虚调用。原具名 self 只发一次（P282 差集
    // ORIG{"16":2} vs OURS{"16":1}）。
    if (this->GetServerType() == EXCHANGE_SERVER_MASTER)
    {
        PacketGuard pkt;
        pkt.clear();
        pkt.put_header(1, 0x83);
        pkt.put_int(GetServerChannelNo());
        pkt.put_int((int)m_serverMap.size());
        for (std::map<int, ExchangeServerInfo>::iterator it =
                 m_serverMap.begin(); it != m_serverMap.end(); ++it)
        {
            pkt.put_int(it->first);
            pkt.put_int(it->second.m_field00);
            pkt.put_short((int)(short)it->second.m_word04);
        }
        pkt.finalize(false);
        m_sessionMgr.TrySend(session, pkt);
    }
    else if (this->GetServerType() == EXCHANGE_SERVER_SLAVE)
    {
        PacketGuard pkt;
        pkt.clear();
        pkt.put_header(1, 0x83);
        pkt.put_int(GetServerChannelNo());
        pkt.put_int(0);
        pkt.finalize(false);
        m_sessionMgr.TrySend(session, pkt);
    }

    if (!RegisterExchangeServer(serverId, info))
    {
        // P227 全表达式临时链：ORIG 0x08492bc1 `mov -0x4c(%ebp),%ebx` 在 ctor 前
        // 把 serverId 载入 callee-saved 并跨 ctor 保活（旧具名形 ctor 后重读槽）。
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvPacketServerInfo(exchange_server::CSession*, "
                 "PacketGuard&)", 0x2c6, 0)
            ("RegisterExchangeServer! failed (%d)", serverId);
    }
    if (!AttachSessionInExchangeServer(serverId, session))
    {
        // P227 同上：ORIG 0x08492c1e `mov -0x4c(%ebp),%ebx` 跨 ctor 保活。
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvPacketServerInfo(exchange_server::CSession*, "
                 "PacketGuard&)", 0x2cb, 0)
            ("AttachSessionInExchangeServer! failed (%d)", serverId);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// OnRecvPacketServerInfoRet                ORIG 0x8492c5e
// Master 广播的 server 表回执：跳过自身表项，其余尝试互连并登记；
// 最后 Attach 发起方会话并转 Slave 角色。
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] 228/228（gap 0，p167 CLEAR）：P227 临时链 + 具名 ipv 复现
// ctor 前 %esi/%ebx/%edi 保活；真 bug 修复=trace 实参序两处（见函数内注释）。
// 残差=N82/N93 槽号差（ORIG -0x84/-0x70/-0x60 vs 我方 -0x80/-0x64/-0x54）封顶。
int CExchangeServer::OnRecvPacketServerInfoRet(CSession* session,
                                               PacketGuard& guard)
{
    // [P330 本泳道 2026-09-09] REAL-BUG 修复（ORIG 0x08492c7f..0x08492c88）：
    // ORIG 在**函数入口**（三个 0 初始化之后、首个 get_int 之前）只调一次
    // `ExchangeServerInfo::ExchangeServerInfo()`（槽 -0x8c），循环体内不再构造
    // （ORIG 全函数该 ctor 出现 1 次，-0x8c 其余 5 处均为取址/读字段）。
    // 我方旧码 `for (...) { ExchangeServerInfo info; ... }` 每轮重新构造 ⇒
    // 未被本轮 get_int/get_short 覆盖的字段被清零；ORIG 则**沿用上一轮残值**
    // 传给 RegisterExchangeServer。p167 抓不到（循环体机器码只有一份，ctor
    // 调用计数两边都是 1）——只有 ctor 调用**位置**（入口 vs 循环内）能发现。
    // 三个 int 的声明序按 ORIG 初始化序 -0x74 / -0x78 / -0x7c 排定。
    // 残余仍为 N82（巨件帧/槽平移）+ N93（cMyTrace 块槽方向）。
    int entryCount = 0;                               // -0x74
    int remoteId = 0;                                 // -0x78
    int senderId = 0;                                 // -0x7c
    ExchangeServerInfo info;                          // -0x8c（ORIG：入口构造一次）

    if (!guard.get_int(senderId))                     // line 0x2d9
    {
        return 0x2d9;
    }
    if (!guard.get_int(entryCount))                   // line 0x2da
    {
        return 0x2da;
    }

    {
        // [b28e 真 bug] ORIG 0x08492d0a/d0x08492d13 压栈序 = senderId(0x8)、
        // entryCount(0xc) ⇒ 实参序 (senderId, entryCount)；旧码两值互换
        // （"channel no(%d), server count(%d)" 打印反）。
        // P227：ORIG 在 ctor 前把两值载入 %esi/%ebx 跨 ctor 保活。
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvPacketServerInfoRet(exchange_server::"
                 "CSession*, PacketGuard&)", 0x2dc, 0)
            ("channel no(%d), server count(%d)", senderId, entryCount);
    }

    for (int i = 0; i < entryCount; ++i)
    {
        if (!guard.get_int(remoteId))                 // line 0x2e0
        {
            return 0x2e0;
        }
        if (!guard.get_int(info.m_field00))           // line 0x2e1
        {
            return 0x2e1;
        }
        // P0：同 OnRecvPacketServerInfo——ORIG 直传 &info.m_word04，无 port16 槽。
        if (!guard.get_short(*(short*)&info.m_word04))  // line 0x2e2
        {
            return 0x2e2;
        }
        info.m_type08 = (CONNECT_TYPE)1;              // ORIG 立即数 1

        // [b28e 真 bug] ORIG 0x08492e60/0x8492e68/0x8492e6e 压栈序 =
        // remoteId(0x8)、ip(0xc)、port(0x10) ⇒ 实参序 (remoteId, ip, port)；
        // 旧码 (ip, remoteId, port) 把 %d 喂指针、%s 喂 int。
        // P0：ORIG 0x08492e4c `mov -0x8c(%ebp),%eax; mov %eax,-0x90(%ebp)` ⇒ 具名 ipv。
        // P227：ip/port/remoteId 在 ctor 前求值（%ebx/%edi/%esi 保活）。
        struct in_addr ipv;
        ipv.s_addr = info.m_field00;
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvPacketServerInfoRet(exchange_server::"
                 "CSession*, PacketGuard&)", 0x2e8, 0)
            ("channel no(%d), server ip(%s), port(%d)", remoteId,
             inet_ntoa(ipv), (int)(short)info.m_word04);

        // [b28d-solo-Exchange] ORIG 0x8492e2b（自身表项）/0x8492e82（find==end，
        // eqERKS5_）/0x8492ebb（TryConnect 成功）三处 `je 8492f15` 与 TryConnect
        // 失败路径全部汇入循环内**无条件** RegisterExchangeServer（0x8492f15）；
        // 旧码写成三处 `continue` 跳过注册 ⇒ p167 eq/ne 差集 + 控制流真差异。
        if (m_localInfo.m_field00 == info.m_field00 &&
            m_localInfo.m_word04 == info.m_word04)
        {
            // 自身表项：不重连，直接注册
        }
        else if (m_serverMap.find(remoteId) == m_serverMap.end())
        {
            // 未登记：不重连，直接注册
        }
        else if (!m_sessionMgr.TryConnect(
                     inet_ntoa(ipv),
                     (int)(short)info.m_word04))
        {
            // P227：ORIG 0x08492ebd/0x8492ec7 在 ctor 前求值 port(%esi)/ip(%ebx)。
            cMyTrace("int exchange_server::CExchangeServer::"
                     "OnRecvPacketServerInfoRet(exchange_server::"
                     "CSession*, PacketGuard&)", 0x2f2, 0)
                ("cannot connect other game server! (%s,%d)",
                 inet_ntoa(ipv), (int)(short)info.m_word04);
        }
        if (!RegisterExchangeServer(remoteId, info))
        {
            // P227：ORIG 0x08492f38 `mov -0x78(%ebp),%ebx` 跨 ctor 保活。
            cMyTrace("int exchange_server::CExchangeServer::"
                     "OnRecvPacketServerInfoRet(exchange_server::"
                     "CSession*, PacketGuard&)", 0x301, 0)
                ("RegisterExchangeServer! failed (%d)", remoteId);
        }
    }

    if (!AttachSessionInExchangeServer(senderId, session))
    {
        // P227：ORIG 0x08492faa `mov -0x7c(%ebp),%ebx` 跨 ctor 保活。
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvPacketServerInfoRet(exchange_server::"
                 "CSession*, PacketGuard&)", 0x307, 0)
            ("AttachSessionInExchangeServer! failed (%d)", senderId);
    }
    SetSlave();
    return 0;
}

// ---------------------------------------------------------------------------
// DispatchPacket                           ORIG 0x8493002
// data[0]=类型（当前仅识别 1），data[1]=子码 0x82..0x85；
// 返回类型 int（ORIG 尾 mov %ebx,%eax 无 bool 归一，0x315 行号直传；
// 调用点 ExchangeServer_Session.cpp 存 int moved 后 cmpl $0）。
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] 129/129（p167 CLEAR）：唯一残差=外层 default 的 pktType
// 求值时机（ORIG 0x8493150 `mov -0x10(%ebp),%ebx` 在 ctor 前，4.4 恒 ctor 后读槽）。
// 已试无效：去 const、`(int)pktType`、语句表达式实参、纯 register（消掉 ORIG 的
// -0x10 槽 ⇒ 帧 0x50→0x40）⇒ 判 N80/P87（单实参 varargs 4.4 不提前求值）封顶。
int CExchangeServer::DispatchPacket(CSession* session, char* data,
                                    int len)
{
    // [b28d-solo-Exchange] 返回类型 bool→int（ORIG 尾 `mov %ebx,%eax` 无 bool
    // 归一、0x315 直传；调用点 Session:177 存 int moved 后 `cmpl $0` 佐证）。
    // case 叶块物理序 = ORIG 地址序 0x84930ab(0x84)/0x84930cb(0x85)/
    // 0x84930eb(0x82)/0x849310b(0x83)；default 与函数尾 return 0 共享
    // `mov $0x0,%ebx` 块（0x84931a8），非 pktCode。
    PacketGuard guard;
    // [b28e] ORIG 0x8493150 `mov -0x10(%ebp),%ebx` 在 cMyTrace ctor 之前读槽并
    // 跨 ctor 保活（P87/P227）；对照 pktCode 的 `(int)pktCode` 实参形使 4.4
    // 提前求值 ⇒ pktType 实参同样显式 cast（裸 register 会消掉 ORIG 的 -0x10 槽）。
    // [b28e-L21 已试无效] P231 纯 register 提示：ORIG 0x8493150 `mov -0x10(%ebp),%ebx`
    // 在 cMyTrace ctor **之前**载入 callee-saved 并跨 ctor 保活；4.4 裸形恒 ctor 后
    // 读槽；加 register 后 4.4 直接**消掉 -0x10 槽**（帧 0x50→0x40）⇒ 更偏离，已回退。
    // 判 N80/P87（单实参 varargs 4.4 不提前求值）。
    int pktType = (signed char)data[0];
    const unsigned char pktCode = (unsigned char)data[1];

    if (!guard.bind_packet(data, len))
    {
        return 0x315;                                 // ORIG ebx=0x315
    }
    guard.set_index(0xf);

    // P286：ORIG 0x849306a `mov -0x10(%ebp),%eax; cmp $0x1,%eax`（载寄存器比较）
    // ⇒ 外层单 case switch（if 形在 4.4 恒折 `cmpl $1,-0x10(%ebp)` 内存直比）。
    switch (pktType)
    {
    case 1:
        switch (pktCode)
        {
        case 0x84:
            return OnRecvPacketServerInfo(session, guard);
        case 0x85:
            return OnRecvPacketServerInfoRet(session, guard);
        case 0x82:
            return OnRecvPacketCharacInfo(session, guard);
        case 0x83:
            return OnRecvPacketCharacInfoRet(session, guard);
        default:
            // P221/P227 临时链：ORIG 0x849312b `movzbl -0x9(%ebp),%ebx` 保活
            // + default 尾 jmp 共享 return 0 块（0x84931a8）⇒ 用 break 出 switch。
            cMyTrace("virtual int exchange_server::CExchangeServer::"
                     "DispatchPacket(exchange_server::CSession*, char*, int)",
                     0x328, 0)
                ("unknown packet code %d", pktCode);
            break;
        }
        break;
    default:
        // P226 裸语句表达式实测无效（4.4 仍 ctor 后读槽）⇒ 判 N7/N18a。
        cMyTrace("virtual int exchange_server::CExchangeServer::"
                 "DispatchPacket(exchange_server::CSession*, "
                 "char*, int)", 0x330, 0)
            ("unknown packet type %d", pktType);
        break;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// OnRecvMessageServerInfo                  ORIG 0x84931de
// stream buffer：+0x0a channel_no(int)、+0x0e port(short)、+0x10 ip(uint)
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] NEAR 判 N7：p167 CLEAR(17/17)；残差=callee-saved 角色
// （ORIG edi=channelNo/esi=port vs 4.4 esi/ebx）+ 缺 push %edi。真 bug 已修：
// SetServerType(MASTER) 移出内层 if（ORIG je 直达）。
int CExchangeServer::OnRecvMessageServerInfo(Stream* stream, int a,
                                             int b)
{
    // [b28d-48] N82 封顶（NDIFF 134，140/142 条）：帧/槽平移 +
    // callee-saved 组差（push edi/esi/ebx vs esi/ebx）。
    (void)a;
    (void)b;
    m_flag39 = true;

    char* buf = stream->get_buffer();
    const ExchangeServerInfoMsgView* sig =
        (const ExchangeServerInfoMsgView*)buf;

    // ORIG 0x8493202/0x849320f：ip 先入具名局部（-0x74，供 inet_ntoa），
    // m_field00 再独立重读一次（P48 O0 无 CSE）。P211 档③：视图字段 unsigned
    // 与 int 目标异型 ⇒ 复现 ORIG `mov 0x10(%eax),%eax; mov %eax,%edx` 中转。
    struct in_addr ipv;
    ipv.s_addr = sig->m_ip10;
    m_localInfo.m_field00 = sig->m_ip10;
    m_localInfo.m_word04 = sig->m_port0e;

    {
        // P227 全表达式临时链：ORIG 0x8493216-0x8493284 在 ctor 前求值
        // channelNo/port/inet_ntoa 并以 %edi/%esi/%ebx 跨 ctor 保活。
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvMessageServerInfo(Stream*, int, int)", 0x343, 0)
            ("Exchange Server IP:%s, PORT:%d, Channel No:%d",
             inet_ntoa(ipv),
             (int)(short)sig->m_port0e, sig->m_channel0a);
    }

    if (sig->m_channel0a == GetServerChannelNo())
    {
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvMessageServerInfo(Stream*, int, int)", 0x347, 0)
            ("This is Exchange Master Server");
        m_flag38 = true;

        // [b28d-solo-Exchange 真 bug] ORIG 0x84932ef `je 8493343` 跳过 trace 直达
        // SetServerType ⇒ SetServerType(MASTER) 在内层 if 之外（旧码写在 if 内，
        // 端口相同时漏调 SetServerType）。
        if (sig->m_port0e != (unsigned short)m_port08)
        {
            cMyTrace("int exchange_server::CExchangeServer::"
                     "OnRecvMessageServerInfo(Stream*, int, int)", 0x34b, 0)
                ("different server port (%d,%d) ",
                 (int)(short)m_port08, (int)(short)sig->m_port0e);
        }
        SetServerType(EXCHANGE_SERVER_MASTER);
    }
    else
    {
        cMyTrace("int exchange_server::CExchangeServer::"
                 "OnRecvMessageServerInfo(Stream*, int, int)", 0x351, 0)
            ("This is Exchange Slave Server");

        if (!m_sessionMgr.TryConnect(
                inet_ntoa(ipv),
                (int)(short)sig->m_port0e))
        {
            cMyTrace("int exchange_server::CExchangeServer::"
                     "OnRecvMessageServerInfo(Stream*, int, int)", 0x354, 0)
                ("cannot connect to other game server!");
        }
        SetServerType(EXCHANGE_SERVER_SLAVE);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// OnRecvMessageCharacInfo                  ORIG 0x849341c
// 本机（参数 a==0x20，ORIG 比较 0x10(%ebp)）走缓存回收路径；否则带请求体转发。
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：真 bug `b==0x20`→`a==0x20`（ORIG 比较 0x10(%ebp)）；
// P198① localHit 两句式预清；P0 内联 length/get_buffer 实参。
int CExchangeServer::OnRecvMessageCharacInfo(Stream* stream, int a, int b)
{
    // [b28d-solo-Exchange] P198① 预清必写：ORIG 首条 `movb $0x0,-0x9` 后条件
    // 置位 `movb $0x1,-0x9`（0x8493423/0x8493451），旧码 const bool 初始化式
    // 产 sete+存槽。P0：fwdLen/fwdBuf 无槽往返 ⇒ 实参内联（ORIG 0x84934a5
    // `mov %eax,%ebx` 跨 get_buffer call 保活，0x84934f7 同）。
    bool localHit = false;
    int body0;                                        // -0x10 先读
    int body1;                                        // -0x14 后读
    *stream >> body0;
    *stream >> body1;

    if (a == 0x20)
    {
        localHit = true;
    }

    if (ReturnCacheCharacter(a, b, body0, body1, localHit, NULL))
    {
        return 0;
    }

    if (SendRequestCharacInfo(a, b, body0, body1, stream->get_buffer(),
                              stream->length()))
    {
        return 0;
    }

    ForwardRequestCharac(a, b, stream->get_buffer(), stream->length());
    return 0;
}

// ---------------------------------------------------------------------------
// DispatchMessage                          ORIG 0x8493530
// msgid 分流（ORIG 决策树实证）：{7..0xc,0x11,0x19,0x20,0xb3,0x282}→CharacInfo；
// {0xb8}→ServerInfo；其余→trace。
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：真 bug 分支映射反转（0xb8→OnRecvMessageServerInfo）+
// P46/P261 switch 化（复现二分决策树）+ P221 临时链。
bool CExchangeServer::DispatchMessage(Stream* stream)
{
    // [b28d-solo-Exchange] P46/P261：ORIG 0x849355e 起为 switch 二分决策树
    // （cmp/je/jg 无状态槽），非 if 链；映射与旧码相反——0xb8 → OnRecvMessage
    // ServerInfo（ORIG 0x849359e 调 0x84931de），{7..0xc,0x11,0x19,0x20,0xb3,
    // 0x282} → OnRecvMessageCharacInfo（ORIG 0x84935c0 调 0x849341c）。
    // 叶块物理序 = case 书写序（ServerInfo 块在前）。
    int msgid;                                        // -0x1c（ORIG 无零初始化）
    int param;                                        // -0x20
    *stream >> msgid;
    *stream >> param;

    switch (msgid)
    {
    case 0xb8:
        OnRecvMessageServerInfo(stream, msgid, param);
        break;
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x11:
    case 0x19:
    case 0x20:
    case 0xb3:
    case 0x282:
        OnRecvMessageCharacInfo(stream, msgid, param);
        break;
    default:
        // P221/P227 全表达式临时链：复现 ORIG 0x84935e2 `mov -0x1c(%ebp),%ebx`
        // （msgid 在 ctor 前物化并跨 ctor 保活）与 trace 槽 -0x18。
        cMyTrace("bool exchange_server::CExchangeServer::"
                 "DispatchMessage(Stream*)", 0x39d, 0)
            ("unknown message type %d", msgid);
        break;
    }
    return true;
}

// ---------------------------------------------------------------------------
// GetMessage                               ORIG 0x849362a（队列 idx=5）
// ---------------------------------------------------------------------------
Stream* CExchangeServer::GetMessage()
{
    return GlobalData::s_msgq_mgr->get(QIdx(5));
}

// ---------------------------------------------------------------------------
// FreeMessage                              ORIG 0x8493648
// 按 Stream.m_poolType(+0x20) 归还池；flag==0 直接放弃。
// ORIG 各越限分支打日志后仍归还原池。
// 判词：返回类型 bool（ORIG 尾 `mov $0x1,%eax`、stream==0 路径 `mov $0x0,%eax`，
// 4.1 不为 void 生成返回值链；ExchangeServer.h 已广播改型，调用点 Infra:572 忽略返回值）。
// [b28d-solo-Exchange] NEAR 判 N93：88=88 条；残差=三臂 cMyTrace 槽深方向
// （ORIG case1 -0x3c/case2 -0x1c vs 4.4 相反）；P221 临时链翻槽成功但 4.4 升格
// len→%ebx（push %ebx）88→91 反劣，已回退。
// ---------------------------------------------------------------------------
bool CExchangeServer::FreeMessage(Stream* stream, int flag)
{
    // [b28-L11 判 N·N93] 助记符序列 88=88 全等，残差 8 = 三 switch 臂 cMyTrace
    // 对象槽深方向跨版本相反（4.1 先声明→-0x3c 深，4.4→-0x1c 浅）。块文本序
    // 即 ORIG 汇编布局序（case1,0,2），交换块序可翻槽号但毁块布局（两约束
    // 死锁）；cMyTrace 带参 ctor 不可 P212 头部声明化。N93 主案在册勿再试。
    if (stream == 0)
    {
        return false;                                 // ORIG: mov $0x0,%eax 后走公共尾
    }

    const int len = stream->length();
    switch (stream->m_poolType)
    {
    case 1:                                           // SmallStreamPool
        if (len > 0xfff)
        {
            // [b28d-solo-Exchange 判 N·N93] P221 临时链实测翻槽方向成功
            // （case1 -0x1c→-0x3c 与 ORIG 同）但副作用：4.4 把 len 升格 %ebx
            // 跨 ctor 保活（push %ebx; sub $0x54；ORIG 无 push、ctor 后重读
            // -0xc），88→91 insn 反劣 ⇒ 维持具名形；残差 4 条 lea 槽号
            // （三臂 cMyTrace 槽深方向跨版本相反，N93 主案）。
            cMyTrace("bool exchange_server::CExchangeServer::"
                     "FreeMessage(Stream*, int)", 0x3b5, 0x5)
                ("Small Stream Size over code(%d), size(%u)", flag, len);
        }
        GlobalData::s_small_stream_pool->Free(stream);
        break;
    case 0:                                           // StreamPool
        if (len > 0x3fff)
        {
            cMyTrace("bool exchange_server::CExchangeServer::"
                     "FreeMessage(Stream*, int)", 0x3c0, 0x5)
                ("Default Stream Size over code(%d), size(%u)", flag, len);
        }
        GlobalData::s_stream_pool->Free(stream);
        break;
    case 2:                                           // BigStreamPool
        if (len > 0x3ffff)
        {
            cMyTrace("bool exchange_server::CExchangeServer::"
                     "FreeMessage(Stream*, int)", 0x3cb, 0x5)
                ("Big Stream Size over code(%d), size(%u)", flag, len);
        }
        GlobalData::s_big_stream_pool->Free(stream);
        break;
    default:
        break;                                        // ORIG: 其它类型直接返回
    }
    return true;
}

// ---------------------------------------------------------------------------
// SendExchangeServerInfo                   ORIG 0x84937ba
// 条件：(!m_flag38 && m_flag39) 或距上次发送 <=9 秒 → 跳过上报，恒返回 true
// （ORIG 正极性守卫 m_flag38||!m_flag39 包裹发送段）。
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：真 bug 两处 return false→true（ORIG 均跳 0x8493848 mov $1）；
// 正极性守卫 `m_flag38||!m_flag39` + time_t 同型相减（jle）+ P52 链式 receiver。
bool CExchangeServer::SendExchangeServerInfo()
{
    // [b28d-solo-Exchange 真 bug] ORIG 两处提前分支都跳 0x8493848 = `mov $0x1,
    // %eax`（返回 true），旧码写 return false（语义反）。另 P52/P0：pProxy 不设
    // 具名局部（ORIG 0x8493816 GetServerProxy 结果直接作 receiver，实参 port/ip
    // 先物化 %edi/%esi 跨 receiver call 保活）。
    // ORIG 正极性守卫 `m_flag38 || !m_flag39`（0x84937cc jne 主流程 / 0x84937da
    // je 共享 return-true 尾 0x8493848），单出口 ⇒ 两处提前分支都跳函数尾 return true。
    if (m_flag38 || !m_flag39)
    {
        const time_t now = time(NULL);
        // ORIG 0x84937fd `jle`（有符号）⇒ time_t 同型相减，勿加 unsigned cast。
        if (now - m_resetTime3c > 9)
        {
            GlobalData::s_monitor_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendExchangeServerInfo(GetServerChannelNo(), m_ip04,
                                         (short)m_port08);
            m_resetTime3c = (unsigned int)now;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// PreparePassiveSocket                     ORIG 0x8493856
// 监听本服端口、把自身注册进 serverMap（即 ORIG 的三级“size over”告警链，
// 其比较立即数为编译期常量——忠实保留判断形态）。
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：真 bug 日志第二实参 sizeProbe→0x3f869/0x3f86b（P97）+
// P221 五臂临时链 + m_ip04 int 化（P211 store 中转）。
bool CExchangeServer::PreparePassiveSocket()
{
    // [b28d-solo-Exchange] P221 临时链翻五臂 trace 槽方向（ORIG 0x40a=-0x60、
    // 0x416=-0x50、0x420=-0x40、0x426=-0x30、0x42c=-0x20 深→浅；4.4 具名形相反）。
    // P97：ORIG 0x8493a0f/0x8493a5e 的 logFormat 第二实参是立即数 0x3f869/0x3f86b
    // （非 sizeProbe），按来源保留。
    m_ip04 = (int)inet_addr(GetServerIP());
    m_port08 = (unsigned short)GetServerPORT();

    if (!m_sessionMgr.TryListen((int)(short)m_port08, GetServerIP()))
    {
        cMyTrace("bool exchange_server::CExchangeServer::"
                 "PreparePassiveSocket()", 0x40a, 0)
            ("TryListen failed!");
        return false;
    }

    {
        ExchangeServerInfo info;
        info.m_field00 = (int)inet_addr(GetServerIP());
        info.m_word04 = m_port08;
        info.m_type08 = EXCHANGE_CONNECT_UNKNOWN;
        if (!RegisterExchangeServer(GetServerChannelNo(), info))
        {
            cMyTrace("bool exchange_server::CExchangeServer::"
                     "PreparePassiveSocket()", 0x416, 0)
                ("RegisterExchangeServer! failed (%d)",
                 GetServerChannelNo());
        }
    }

    // ORIG 局部序列：-0x10=0x10（未参与比较）、-0xc=0x15469 后接三级常量门限
    // 注意：不能声明 const（GCC O0 会常量折叠删除比较链，ORIG 保留运行时判断）。
    int warnBase = 0x10;                              // -0x10（写入未读取）
    (void)warnBase;
    int sizeProbe = 0x15469;                          // -0xc
    if (sizeProbe > 0x17318)
    {
        cMyTrace("bool exchange_server::CExchangeServer::"
                 "PreparePassiveSocket()", 0x420, 0)
            ("RegisterExchangeServer! failed packet size over! (%d/%d)",
             sizeProbe, 0x17318);
    }
    if (sizeProbe > 0x17318)
    {
        cMyTrace("bool exchange_server::CExchangeServer::"
                 "PreparePassiveSocket()", 0x426, 0)
            ("RegisterExchangeServer! failed recv buffer size over! "
             "(%d/%d)", 0x3f869, 0x17318);
    }
    if (sizeProbe > 0x6ddd0)
    {
        cMyTrace("bool exchange_server::CExchangeServer::"
                 "PreparePassiveSocket()", 0x42c, 0)
            ("RegisterExchangeServer! failed send buffer size over! "
             "(%d/%d)", 0x3f86b, 0x6ddd0);
    }
    return true;
}

// ---------------------------------------------------------------------------
// C1 / D1                                  ORIG 0x8493ae6 / 0x8493b82
// 成员按地址序构造：sessionMgr(+0x0c) → localInfo(+0x40) → map(+0x50)
// → pool(+0x6c)，尾调 Reset。析构逆序由默认成员析构承担。
// ---------------------------------------------------------------------------
// ORIG 0x8493ae6：仅 IExchangeServer 基类 + 成员按序默认构造（sessionMgr
// → localInfo → map → pool）→ Reset()。标量成员（+0x04/+0x08/+0x0a/+0x38/
// +0x39/+0x3c/+0x68）不初始化（ORIG 无清零指令）。
CExchangeServer::CExchangeServer()
    : IExchangeServer(),
      m_sessionMgr(),
      m_localInfo(),
      m_serverMap(),
      m_sessionPool()
{
    Reset();
}

CExchangeServer::~CExchangeServer()
{
}

}  // namespace exchange_server
