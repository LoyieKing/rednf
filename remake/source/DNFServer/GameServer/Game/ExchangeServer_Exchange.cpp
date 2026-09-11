#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ExchangeServer_Exchange/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "ExchangeServer_Exchange/ExchangeServerInfo.inc"


#include "ExchangeServer_Exchange/GetServerType.inc"


#include "ExchangeServer_Exchange/SetServerType.inc"


#include "ExchangeServer_Exchange/AllocSession.inc"


#include "ExchangeServer_Exchange/FreeSession.inc"


#include "ExchangeServer_Exchange/Reset.inc"


#include "ExchangeServer_Exchange/GetNetworkEvent.inc"


#include "ExchangeServer_Exchange/GetExchangeServerCount.inc"


#include "ExchangeServer_Exchange/SetMaster.inc"


#include "ExchangeServer_Exchange/SetSlave.inc"


#include "ExchangeServer_Exchange/TrySend.inc"


#include "ExchangeServer_Exchange/AttachSessionInExchangeServer.inc"


#include "ExchangeServer_Exchange/RegisterExchangeServer.inc"


#include "ExchangeServer_Exchange/UnregisterExchangeServer.inc"


#include "ExchangeServer_Exchange/SendRequestCharacInfo.inc"


#include "ExchangeServer_Exchange/SendResponseCharacInfo.inc"


#include "ExchangeServer_Exchange/SendResponseCharacInfo_1.inc"


#include "ExchangeServer_Exchange/SendResponseCharacInfo_forchina.inc"


#include "ExchangeServer_Exchange/ReturnCacheCharacter.inc"


#include "ExchangeServer_Exchange/ForwardRequestCharac_forchina.inc"


#include "ExchangeServer_Exchange/ForwardRequestCharac.inc"


#include "ExchangeServer_Exchange/OnRecvPacketCharacInfo.inc"


#include "ExchangeServer_Exchange/OnRecvPacketCharacInfoRet.inc"


#include "ExchangeServer_Exchange/OnRecvPacketServerInfo.inc"


#include "ExchangeServer_Exchange/OnRecvPacketServerInfoRet.inc"


#include "ExchangeServer_Exchange/DispatchPacket.inc"


#include "ExchangeServer_Exchange/OnRecvMessageServerInfo.inc"
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
        cMyTrace(__PRETTY_FUNCTION__, 0x343, 0)
            ("Exchange Server IP:%s, PORT:%d, Channel No:%d",
             inet_ntoa(ipv),
             (int)(short)sig->m_port0e, sig->m_channel0a);
    }

    if (sig->m_channel0a == GetServerChannelNo())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x347, 0)
            ("This is Exchange Master Server");
        m_flag38 = true;

        // [b28d-solo-Exchange 真 bug] ORIG 0x84932ef `je 8493343` 跳过 trace 直达
        // SetServerType ⇒ SetServerType(MASTER) 在内层 if 之外（旧码写在 if 内，
        // 端口相同时漏调 SetServerType）。
        if (sig->m_port0e != (unsigned short)m_port08)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x34b, 0)
                ("different server port (%d,%d) ",
                 (int)(short)sig->m_port0e, (int)(short)m_port08);
        }
        SetServerType(EXCHANGE_SERVER_MASTER);
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x351, 0)
            ("This is Exchange Slave Server");

        if (!m_sessionMgr.TryConnect(
                inet_ntoa(ipv),
                (int)(short)sig->m_port0e))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x354, 0)
                ("cannot connect to other game server!");
        }
        SetServerType(EXCHANGE_SERVER_SLAVE);
    }
    return 0;
}

#include "ExchangeServer_Exchange/OnRecvMessageCharacInfo.inc"


#include "ExchangeServer_Exchange/DispatchMessage.inc"


#include "ExchangeServer_Exchange/GetMessage.inc"


#include "ExchangeServer_Exchange/FreeMessage.inc"


#include "ExchangeServer_Exchange/SendExchangeServerInfo.inc"


#include "ExchangeServer_Exchange/PreparePassiveSocket.inc"


#include "ExchangeServer_Exchange/CExchangeServer.inc"


#include "ExchangeServer_Exchange/dtor_CExchangeServer.inc"


}  // namespace exchange_server
