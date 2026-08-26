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
    m_serverType68 = (int)type;
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
    m_resetTime3c = (unsigned int)time(NULL);
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
    for (std::map<int, ExchangeServerInfo>::iterator it = m_serverMap.begin();
         it != m_serverMap.end(); ++it)
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
        "void exchange_server::CExchangeServer::SetMaster()", 0xf6, 0);
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
        "void exchange_server::CExchangeServer::SetSlave()", 0xfe, 0);
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
    if (it == m_serverMap.end() || it->second.m_pSession == NULL)
    {
        return false;
    }
    return m_sessionMgr.TrySend(it->second.m_pSession, guard);
}

// ---------------------------------------------------------------------------
// AttachSessionInExchangeServer            ORIG 0x8491296
// ---------------------------------------------------------------------------
bool CExchangeServer::AttachSessionInExchangeServer(int serverId,
                                                    CSession* session)
{
    session->SetChannelNo(serverId);
    std::map<int, ExchangeServerInfo>::iterator it =
        m_serverMap.find(serverId);
    if (it != m_serverMap.end())
    {
        it->second.m_pSession = session;
        return true;
    }
    cMyTrace trace("bool exchange_server::CExchangeServer::"
                   "AttachSessionInExchangeServer(int, "
                   "exchange_server::CSession*)", 0x11b, 0);
    trace("AttachSessionInExchangeServer() channel_no(%d) failed", serverId);
    return false;
}

// ---------------------------------------------------------------------------
// RegisterExchangeServer                   ORIG 0x849135c
// ---------------------------------------------------------------------------
bool CExchangeServer::RegisterExchangeServer(int serverId,
                                             ExchangeServerInfo& info)
{
    if (m_serverMap.find(serverId) == m_serverMap.end())
    {
        m_serverMap.insert(std::make_pair(serverId, info));
        return true;
    }
    cMyTrace trace("bool exchange_server::CExchangeServer::"
                   "RegisterExchangeServer(int, "
                   "exchange_server::ExchangeServerInfo&)", 0x128, 0);
    trace("RegisterExchangeServer() channel_no(%d) failed", serverId);
    return false;
}

// ---------------------------------------------------------------------------
// UnregisterExchangeServer                 ORIG 0x849144c
// （移除后若无 SLAVE（CONNECT_TYPE=1）剩留则自动转 Master）
// ---------------------------------------------------------------------------
void CExchangeServer::UnregisterExchangeServer(int serverId)
{
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
        return;
    }
    cMyTrace trace("virtual bool exchange_server::CExchangeServer::"
                   "UnregisterExchangeServer(int)", 0x13b, 0);
    trace("UnregisterExchangeServer() channel_no(%d) failed", serverId);
    return;
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
    return TrySend(serverId, guard);
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
    return SendResponseCharacInfo_forchina(a, b, data, len);
}

// ---------------------------------------------------------------------------
// SendResponseCharacInfo_forchina          ORIG 0x849176e
// 三档池取流（total=len+8：<=0xfff Small / <=0x3fff Default / else Big），
// 写 a、b、binary(data) 后投递 MsgQueueMgr idx=1（DISPATCH_Q）。
// 注：ORIG 返回值取 MsgQueueMgr::put 尾部 eax；重建 put 为 void 转发，
// 此处按成功路径返回 true（ORIG 观测值不可考证项，见交付清单）。
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
    else if (total <= 0x3fff)
    {
        sGuard.Set(GlobalData::s_stream_pool->Acquire("ExchangeServer.cpp", 0x17e));
    }
    else
    {
        sGuard.Set(GlobalData::s_big_stream_pool->Acquire("ExchangeServer.cpp",
                                              0x182));
    }

    sGuard << a;
    sGuard << b;
    sGuard.put_binary(data, len);

    GlobalData::s_msgq_mgr->put(QIdx(1), sGuard);
    return true;
}

// ---------------------------------------------------------------------------
// ReturnCacheCharacter                     ORIG 0x84918d8
// 本 channel 命中缓存并回包；参数 e 控制命中后是否归还池内存。
// ---------------------------------------------------------------------------
bool CExchangeServer::ReturnCacheCharacter(int a, int b, int c, int d,
                                           bool e, CSession* session)
{
    bool handled = false;
    if (GetServerChannelNo() == d)
    {
        CACHE_CHARACTER_TYPE* pCache = NULL;
        CCacheCharacterMgr* mgr = GetInstanceCacheCharacterMgr();
        if (!mgr->GetCacheCharacter(c, &pCache, e))
        {
            cMyTrace trace("bool exchange_server::CExchangeServer::"
                           "ReturnCacheCharacter(int, int, int, int, bool, "
                           "exchange_server::CSession*)", 0x1d6, 0);
            trace("GetCacheCharacter() false  type(%d), idx(%d), "
                  "channel_no(%d), charac_no(%d)", a, b, c, d);
            return handled;
        }

        int pos = 0;
        int memberIdx = 0;
        bool found = false;
        pos = pCache->GetMemberPosition((ENUM_INTERNALPACKET)a,
                                        &memberIdx, found);
        const bool valid = (pos != 0 && found);
        if (!valid)
        {
            cMyTrace trace("bool exchange_server::CExchangeServer::"
                           "ReturnCacheCharacter(int, int, int, int, bool, "
                           "exchange_server::CSession*)", 0x1bf, 0);
            trace("GetMemberPosition() invalid information  type(%d), "
                  "idx(%d), channel_no(%d), charac_no(%d)", a, b, c, d);
        }
        else
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
        if (e)
        {
            mgr->FreeCharacter(pCache);
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
    if (len == 0x20)
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
    else if (total <= 0x3fff)
    {
        sGuard.Set(GlobalData::s_stream_pool->Acquire("ExchangeServer.cpp", 0x1f0));
    }
    else
    {
        sGuard.Set(GlobalData::s_big_stream_pool->Acquire("ExchangeServer.cpp",
                                              0x1f4));
    }

    sGuard << a;
    sGuard << b;
    sGuard.put_binary(data, len);

    GlobalData::s_msgq_mgr->put(QIdx(2), sGuard);
    return true;
}

// ---------------------------------------------------------------------------
// ForwardRequestCharac                     ORIG 0x8491c32 → _forchina
// ---------------------------------------------------------------------------
bool CExchangeServer::ForwardRequestCharac(int a, int b, void* data,
                                           int len)
{
    return ForwardRequestCharac_forchina(a, b, data, len);
}

// ---------------------------------------------------------------------------
// OnRecvPacketCharacInfo                   ORIG 0x84922ee（返回失败行号/0）
// 包字段：type, idx, channel_no, charac_no[, msg_size, binary]
// ---------------------------------------------------------------------------
int CExchangeServer::OnRecvPacketCharacInfo(CSession* session,
                                            PacketGuard& guard)
{
    int errBook = 0;                                  // -0x44 (类型 a)
    int idx = 0;                                      // -0x48
    int chNoArg = 0;                                  // -0x4c
    int characNo = 0;                                 // -0x50

    if (!guard.get_int(errBook))                      // fail line 0x235
    {
        errBook = 0x235;
        CACHE_CHARACTER_TYPE cc;                      // goto-epilogue 构造序
        return errBook;
    }
    if (!guard.get_int(idx))
    {
        errBook = 0x236;
        CACHE_CHARACTER_TYPE cc;
        return errBook;
    }
    if (!guard.get_int(chNoArg))
    {
        errBook = 0x237;
        CACHE_CHARACTER_TYPE cc;
        return errBook;
    }
    if (!guard.get_int(characNo))
    {
        errBook = 0x238;
        CACHE_CHARACTER_TYPE cc;
        return errBook;
    }

    // ORIG：大对象置于栈帧 -0x3f8b5（sizeof(CACHE_CHARACTER_TYPE)=0x3f85c）
    CACHE_CHARACTER_TYPE cc;
    int pos = 0;                                      // -0x1c
    int mi1 = 0;                                      // -0x54
    int mi2 = 0;                                      // -0x58
    bool found = false;                               // -0x59
    pos = cc.GetMemberPosition((ENUM_INTERNALPACKET)errBook, &mi1, found);

    if (pos != 0)
    {
        if (!guard.get_int(mi2))                      // line 0x241
        {
            return 0x241;
        }
        if (!guard.get_binary((char*)pos, mi2))       // line 0x24d
        {
            return 0x24d;
        }
    }

    const int fromChannel = session->GetChannelNo();
    {
        cMyTrace trace("int exchange_server::CExchangeServer::"
                       "OnRecvPacketCharacInfo(exchange_server::CSession*, "
                       "PacketGuard&)", 0x253, 0);
        trace("OnRecvPacketCharacInfo() type(%d), idx(%d), msg len(%d), "
              "member_size(%d), channel_no(%d), charac_no(%d), "
              "from_channel_no",
              errBook, idx, mi2, mi1, chNoArg, characNo, fromChannel);
    }

    const bool localHit = (errBook == 0x20);          // -0x1d

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
int CExchangeServer::OnRecvPacketCharacInfoRet(CSession* session,
                                               PacketGuard& guard)
{
    int ret = 0;                                      // -0x40
    int idx = 0;                                      // -0x38
    int fromCh = 0;                                   // -0x3c

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

    if (pos != 0)
    {
        if (!guard.get_int(mi2))                      // line 0x27a
        {
            return 0x27a;
        }
        if (!guard.get_binary((char*)pos, mi2))       // line 0x282
        {
            return 0x282;
        }
    }

    const int channel = session->GetChannelNo();
    {
        cMyTrace trace("int exchange_server::CExchangeServer::"
                       "OnRecvPacketCharacInfoRet(exchange_server::"
                       "CSession*, PacketGuard&)", 0x28a, 0);
        trace("OnRecvPacketCharacInfoRet() ret(%d) type(%d), idx(%d), "
              "msg len(%d), member_size(%d), from_channel(%d)",
              ret, idx, fromCh, mi2, mi1, channel);
    }

    if (ret == 0)
    {
        if (!SendResponseCharacInfo(idx, fromCh, (void*)pos, mi1))
        {
            ForwardRequestCharac(idx, fromCh, (void*)pos, mi1);
        }
    }
    else
    {
        ForwardRequestCharac(idx, fromCh, (void*)pos, mi1);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// OnRecvPacketServerInfo                   ORIG 0x8492854
// 包字段：serverId, ip, port(short)；Master 回全表、Slave 回空注册响应包
// ---------------------------------------------------------------------------
int CExchangeServer::OnRecvPacketServerInfo(CSession* session,
                                            PacketGuard& guard)
{
    ExchangeServerInfo info;
    int serverId = 0;

    if (!guard.get_int(serverId))                     // line 0x29d
    {
        return 0x29d;
    }
    if (!guard.get_int(info.m_field00))               // line 0x29e
    {
        return 0x29e;
    }
    short port16 = 0;
    if (!guard.get_short(port16))                     // line 0x29f
    {
        return 0x29f;
    }
    info.m_word04 = (unsigned short)port16;
    info.m_type08 = (CONNECT_TYPE)2;                  // ORIG 立即数 2

    {
        cMyTrace trace("int exchange_server::CExchangeServer::"
                       "OnRecvPacketServerInfo(exchange_server::CSession*, "
                       "PacketGuard&)", 0x2a5, 0);
        trace("channel no(%d), server ip(%s), port(%d)",
              inet_ntoa(*(struct in_addr*)&info.m_field00), serverId,
              (int)(short)info.m_word04);
    }

    const EXCHANGE_SERVER_TYPE self = this->GetServerType();  // 虚调用 slot4
    if (self == EXCHANGE_SERVER_MASTER)
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
    else if (self == EXCHANGE_SERVER_SLAVE)
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
        cMyTrace trace("int exchange_server::CExchangeServer::"
                       "OnRecvPacketServerInfo(exchange_server::CSession*, "
                       "PacketGuard&)", 0x2c6, 0);
        trace("RegisterExchangeServer! failed (%d)", serverId);
    }
    if (!AttachSessionInExchangeServer(serverId, session))
    {
        cMyTrace trace("int exchange_server::CExchangeServer::"
                       "OnRecvPacketServerInfo(exchange_server::CSession*, "
                       "PacketGuard&)", 0x2cb, 0);
        trace("AttachSessionInExchangeServer! failed (%d)", serverId);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// OnRecvPacketServerInfoRet                ORIG 0x8492c5e
// Master 广播的 server 表回执：跳过自身表项，其余尝试互连并登记；
// 最后 Attach 发起方会话并转 Slave 角色。
// ---------------------------------------------------------------------------
int CExchangeServer::OnRecvPacketServerInfoRet(CSession* session,
                                               PacketGuard& guard)
{
    int entryCount = 0;                               // -0x74
    int senderId = 0;                                 // -0x7c

    if (!guard.get_int(senderId))                     // line 0x2d9
    {
        return 0x2d9;
    }
    if (!guard.get_int(entryCount))                   // line 0x2da
    {
        return 0x2da;
    }

    {
        cMyTrace trace("int exchange_server::CExchangeServer::"
                       "OnRecvPacketServerInfoRet(exchange_server::"
                       "CSession*, PacketGuard&)", 0x2dc, 0);
        trace("channel no(%d), server count(%d)", entryCount, senderId);
    }

    for (int i = 0; i < entryCount; ++i)
    {
        ExchangeServerInfo info;
        int remoteId = 0;                             // -0x78
        if (!guard.get_int(remoteId))                 // line 0x2e0
        {
            return 0x2e0;
        }
        if (!guard.get_int(info.m_field00))           // line 0x2e1
        {
            return 0x2e1;
        }
        short port16 = 0;
        if (!guard.get_short(port16))                 // line 0x2e2
        {
            return 0x2e2;
        }
        info.m_word04 = (unsigned short)port16;
        info.m_type08 = (CONNECT_TYPE)1;              // ORIG 立即数 1

        {
            cMyTrace trace("int exchange_server::CExchangeServer::"
                           "OnRecvPacketServerInfoRet(exchange_server::"
                           "CSession*, PacketGuard&)", 0x2e8, 0);
            trace("channel no(%d), server ip(%s), port(%d)",
                  inet_ntoa(*(struct in_addr*)&info.m_field00), remoteId,
                  (int)(short)info.m_word04);
        }

        // 自身表项（ip+port 相同）不重连
        if (m_localInfo.m_field00 == info.m_field00 &&
            m_localInfo.m_word04 == info.m_word04)
        {
            continue;
        }
        if (m_serverMap.find(remoteId) != m_serverMap.end())
        {
            continue;                                 // 已登记
        }
        if (m_sessionMgr.TryConnect(
                inet_ntoa(*(struct in_addr*)&info.m_field00),
                (int)(short)info.m_word04))
        {
            continue;                                 // 连接成功
        }
        cMyTrace trace("int exchange_server::CExchangeServer::"
                       "OnRecvPacketServerInfoRet(exchange_server::"
                       "CSession*, PacketGuard&)", 0x2f2, 0);
        trace("cannot connect other game server! (%s,%d)",
              inet_ntoa(*(struct in_addr*)&info.m_field00),
              (int)(short)info.m_word04);
        RegisterExchangeServer(remoteId, info);       // 失败亦登记占位
    }

    if (!AttachSessionInExchangeServer(senderId, session))
    {
        cMyTrace trace("int exchange_server::CExchangeServer::"
                       "OnRecvPacketServerInfoRet(exchange_server::"
                       "CSession*, PacketGuard&)", 0x307, 0);
        trace("AttachSessionInExchangeServer! failed (%d)", senderId);
    }
    SetSlave();
    return 0;
}

// ---------------------------------------------------------------------------
// DispatchPacket                           ORIG 0x8493002
// data[0]=类型（当前仅识别 1），data[1]=子码 0x82..0x85；
// 返回值语义＝子处理器返回行号等 int 的低字节布尔（绑定失败视作真——
// 与 ORIG al 寄存器观测一致）。
// ---------------------------------------------------------------------------
bool CExchangeServer::DispatchPacket(CSession* session, char* data,
                                     int len)
{
    PacketGuard guard;
    const int pktType = (signed char)data[0];
    const unsigned char pktCode = (unsigned char)data[1];

    if (!guard.bind_packet(data, len))
    {
        return ((bool)0x315);                         // ORIG ebx=0x315
    }
    guard.set_index(0xf);

    int retErr = 0;
    if (pktType == 1)
    {
        switch (pktCode)
        {
        case 0x82:
            retErr = OnRecvPacketCharacInfo(session, guard);
            break;
        case 0x83:
            retErr = OnRecvPacketCharacInfoRet(session, guard);
            break;
        case 0x84:
            retErr = OnRecvPacketServerInfo(session, guard);
            break;
        case 0x85:
            retErr = OnRecvPacketServerInfoRet(session, guard);
            break;
        default:
            {
                cMyTrace trace("virtual int exchange_server::"
                               "CExchangeServer::DispatchPacket("
                               "exchange_server::CSession*, char*, int)",
                               0x328, 0);
                trace("unknown packet code %d", (int)pktCode);
                retErr = (int)pktCode;                // ORIG 保留 sub 字节
            }
            break;
        }
    }
    else
    {
        cMyTrace trace("virtual int exchange_server::CExchangeServer::"
                       "DispatchPacket(exchange_server::CSession*, "
                       "char*, int)", 0x330, 0);
        trace("unknown packet type %d", pktType);
        retErr = 0;
    }
    return (retErr != 0);
}

// ---------------------------------------------------------------------------
// OnRecvMessageServerInfo                  ORIG 0x84931de
// stream buffer：+0x0a channel_no(int)、+0x0e port(short)、+0x10 ip(uint)
// ---------------------------------------------------------------------------
void CExchangeServer::OnRecvMessageServerInfo(Stream* stream, int a,
                                              int b)
{
    (void)a;
    (void)b;
    m_flag39 = true;

    char* buf = stream->get_buffer();

    m_localInfo.m_field00 = *(int*)(buf + 0x10);      // ip
    unsigned short recvPort =
        *(unsigned short*)(buf + 0x0e);               // signext 参与打印/比较
    m_localInfo.m_word04 = recvPort;

    {
        cMyTrace trace("int exchange_server::CExchangeServer::"
                       "OnRecvMessageServerInfo(Stream*, int, int)",
                       0x343, 0);
        trace("Exchange Server IP:%s, PORT:%d, Channel No:%d",
              inet_ntoa(*(struct in_addr*)&m_localInfo.m_field00),
              (int)(short)recvPort, *(int*)(buf + 0x0a));
    }

    if (*(int*)(buf + 0x0a) == GetServerChannelNo())
    {
        cMyTrace trace("int exchange_server::CExchangeServer::"
                       "OnRecvMessageServerInfo(Stream*, int, int)",
                       0x347, 0);
        trace("This is Exchange Master Server");
        m_flag38 = true;

        if ((unsigned short)m_port08 != recvPort)
        {
            cMyTrace trace("int exchange_server::CExchangeServer::"
                           "OnRecvMessageServerInfo(Stream*, int, int)",
                           0x34b, 0);
            trace("different server port (%d,%d) ",
                  (int)(short)m_port08, (int)(short)recvPort);
            SetServerType(EXCHANGE_SERVER_MASTER);
        }
    }
    else
    {
        cMyTrace trace("int exchange_server::CExchangeServer::"
                       "OnRecvMessageServerInfo(Stream*, int, int)",
                       0x351, 0);
        trace("This is Exchange Slave Server");

        const bool ok = m_sessionMgr.TryConnect(
            inet_ntoa(*(struct in_addr*)&m_localInfo.m_field00),
            (int)(short)recvPort);
        if (!ok)
        {
            cMyTrace trace("int exchange_server::CExchangeServer::"
                           "OnRecvMessageServerInfo(Stream*, int, int)",
                           0x354, 0);
            trace("cannot connect to other game server!");
        }
        SetServerType(EXCHANGE_SERVER_SLAVE);
    }
}

// ---------------------------------------------------------------------------
// OnRecvMessageCharacInfo                  ORIG 0x849341c
// 本机（参数 b==0x20）走缓存回收路径；否则带请求体转发给归属服务器。
// ---------------------------------------------------------------------------
int CExchangeServer::OnRecvMessageCharacInfo(Stream* stream, int a, int b)
{
    int body0 = 0;                                    // -0x10 先读
    int body1 = 0;                                    // -0x14 后读
    *stream >> body0;
    *stream >> body1;

    const bool localHit = (b == 0x20);
    if (ReturnCacheCharacter(a, b, body0, body1, localHit, NULL))
    {
        return 0;
    }

    {
        const int fwdLen = stream->length();
        char* fwdBuf = stream->get_buffer();
        if (SendRequestCharacInfo(a, b, body0, body1, fwdBuf, fwdLen))
        {
            return 0;
        }
    }

    {                                                 // ForwardRequestCharac 兜底
        const int fwdLen = stream->length();
        char* fwdBuf = stream->get_buffer();
        ForwardRequestCharac(a, b, fwdBuf, fwdLen);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// DispatchMessage                          ORIG 0x8493530
// msgid 分流：{≤0xc,0x11,0x19,0x20,0xb3}→ServerInfo；{0xb8,0x282}→CharacInfo
// ---------------------------------------------------------------------------
bool CExchangeServer::DispatchMessage(Stream* stream)
{
    int msgid = 0;                                    // -0x1c
    int param = 0;                                    // -0x20
    *stream >> msgid;
    *stream >> param;

    if (msgid <= 0xc || msgid == 0x11 || msgid == 0x19 ||
        msgid == 0x20 || msgid == 0xb3)
    {
        OnRecvMessageServerInfo(stream, msgid, param);
    }
    else if (msgid == 0xb8 || msgid == 0x282)
    {
        OnRecvMessageCharacInfo(stream, msgid, param);
    }
    else
    {
        cMyTrace trace("bool exchange_server::CExchangeServer::"
                       "DispatchMessage(Stream*)", 0x39d, 0);
        trace("unknown message type %d", msgid);
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
// 注：头声明 void，ORIG 尾部 eax=0/1（归 false/true），无调用方消费，
// 行为主体已完整保留。
// ---------------------------------------------------------------------------
void CExchangeServer::FreeMessage(Stream* stream, int flag)
{
    if (flag == 0)
    {
        return;                                       // ORIG: 返回 false 不释放
    }

    const int len = stream->length();
    switch (stream->m_poolType)
    {
    case 1:                                           // SmallStreamPool
        if (len > 0xfff)
        {
            cMyTrace trace("bool exchange_server::CExchangeServer::"
                           "FreeMessage(Stream*, int)", 0x3b5, 0);
            trace("Small Stream Size over code(%d), size(%u)", flag,
                  (unsigned int)len);
        }
        GlobalData::s_small_stream_pool->Free(stream);
        break;
    case 0:                                           // StreamPool
        if (len > 0x3fff)
        {
            cMyTrace trace("bool exchange_server::CExchangeServer::"
                           "FreeMessage(Stream*, int)", 0x3c0, 0);
            trace("Default Stream Size over code(%d), size(%u)", flag,
                  (unsigned int)len);
        }
        GlobalData::s_stream_pool->Free(stream);
        break;
    case 2:                                           // BigStreamPool
        if (len > 0x3ffff)
        {
            cMyTrace trace("bool exchange_server::CExchangeServer::"
                           "FreeMessage(Stream*, int)", 0x3cb, 0);
            trace("Big Stream Size over code(%d), size(%u)", flag,
                  (unsigned int)len);
        }
        GlobalData::s_big_stream_pool->Free(stream);
        break;
    default:
        break;                                        // ORIG: 其它类型直接返回
    }
    return;
}

// ---------------------------------------------------------------------------
// SendExchangeServerInfo                   ORIG 0x84937ba
// 条件：(!m_flag38 && !m_flag39) 且距上次发送 >9 秒 → 经 Monitor 上报本服信息
// ---------------------------------------------------------------------------
bool CExchangeServer::SendExchangeServerInfo()
{
    if (!m_flag38 && m_flag39)
    {
        return false;                                 // ORIG: flag39 恒走公共尾
    }
    const time_t now = time(NULL);
    if ((unsigned int)now - m_resetTime3c <= 9)
    {
        return false;
    }
    CMonitorServerProxy* pProxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0);
    pProxy->SendExchangeServerInfo(GetServerChannelNo(), (int)m_ip04,
                                   (short)m_port08);
    m_resetTime3c = (unsigned int)now;
    return true;
}

// ---------------------------------------------------------------------------
// PreparePassiveSocket                     ORIG 0x8493856
// 监听本服端口、把自身注册进 serverMap（即 ORIG 的三级“size over”告警链，
// 其比较立即数为编译期常量——忠实保留判断形态）。
// ---------------------------------------------------------------------------
bool CExchangeServer::PreparePassiveSocket()
{
    m_ip04 = (unsigned int)inet_addr(GetServerIP());
    m_port08 = (unsigned short)GetServerPORT();

    if (!m_sessionMgr.TryListen((int)(short)m_port08, GetServerIP()))
    {
        cMyTrace trace("bool exchange_server::CExchangeServer::"
                       "PreparePassiveSocket()", 0x40a, 0);
        trace("TryListen failed!");
        return false;
    }

    {
        ExchangeServerInfo info;
        info.m_field00 = (int)inet_addr(GetServerIP());
        info.m_word04 = m_port08;
        info.m_type08 = EXCHANGE_CONNECT_UNKNOWN;
        if (!RegisterExchangeServer(GetServerChannelNo(), info))
        {
            cMyTrace trace("bool exchange_server::CExchangeServer::"
                           "PreparePassiveSocket()", 0x416, 0);
            trace("RegisterExchangeServer! failed (%d)",
                  GetServerChannelNo());
        }
    }

    // ORIG 局部序列：-0x10=0x10（未参与比较）、-0xc=0x15469 后接三级常量门限
    const int warnBase = 0x10;                        // -0x10（写入未读取）
    (void)warnBase;
    const int sizeProbe = 0x15469;                    // -0xc
    if (sizeProbe > 0x17318)
    {
        cMyTrace trace("bool exchange_server::CExchangeServer::"
                       "PreparePassiveSocket()", 0x420, 0);
        trace("RegisterExchangeServer! failed packet size over! (%d/%d)",
              sizeProbe, 0x17318);
    }
    if (sizeProbe > 0x17318)
    {
        cMyTrace trace("bool exchange_server::CExchangeServer::"
                       "PreparePassiveSocket()", 0x426, 0);
        trace("RegisterExchangeServer! failed recv buffer size over! "
              "(%d/%d)", sizeProbe, 0x17318);
    }
    if (sizeProbe > 0x6ddd0)
    {
        cMyTrace trace("bool exchange_server::CExchangeServer::"
                       "PreparePassiveSocket()", 0x42c, 0);
        trace("RegisterExchangeServer! failed send buffer size over! "
              "(%d/%d)", sizeProbe, 0x6ddd0);
    }
    return true;
}

// ---------------------------------------------------------------------------
// C1 / D1                                  ORIG 0x8493ae6 / 0x8493b82
// 成员按地址序构造：sessionMgr(+0x0c) → localInfo(+0x40) → map(+0x50)
// → pool(+0x6c)，尾调 Reset。析构逆序由默认成员析构承担。
// ---------------------------------------------------------------------------
CExchangeServer::CExchangeServer()
    : IExchangeServer(),
      m_ip04(0),
      m_port08(0),
      m_pad0a(),
      m_sessionMgr(),
      m_flag38(false),
      m_flag39(false),
      m_resetTime3c(0),
      m_localInfo(),
      m_serverMap(),
      m_serverType68(0),
      m_sessionPool()
{
    Reset();
}

CExchangeServer::~CExchangeServer()
{
}

}  // namespace exchange_server
