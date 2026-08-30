// ============================================================================
// df_game_r exchange_server::CSession 全方法（OnlyOrig_Exchange 批；
// TUID ExchangeServer_Session.cpp，需登记 CMake）
//
// 反汇编快照 /tmp/es_disasm.txt 行 869-1745 区段逐指令转写。
//
//   布局锚点：CSession sizeof = sizeof(CNetwork<95000,450000>) + 8
//     +0x851a0 m_triggerEventType   +0x851a4 m_channelNo
//
// IExchangeServer 虚槽序（vtable 派生 0x8c7a1d0 / 抽象 0x8c7a1e8）：
//   +0 DispatchPacket(CSession*,char*,int) / +4 UnregisterExchangeServer(int)
//   +8 AllocSession() / +12 FreeSession(CSession*) / +16 DispatchMessage(Stream*)
// 全局 exchange_server_E(0x943d9f4, B) 在 ORIG 存的是内嵌 server 子对象地址
// （thread this+0xc），即已是 IExchangeServer* 视角指针；ORIG 反汇编
// `mov (%eax),%eax` 为函数级编译产物中的二次解引（thread*→server 子对象），
// 本 TU 以 GetExchangeServerInterface() 显式表达该转换。
//
// 无 BLOCKED 函数。
// ============================================================================

#include "ExchangeServer.h"
#include "CNetwork.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "LogManager.h"
#include "GlobalData.h"           // GlobalData::s_systemTime_（get_ms_tick）
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
// ---- get_ms_tick（ORIG _Z11get_ms_tickv @0x086b1b2f；报告
//      docs/class_func_reports/_lt_global_gt_/get_ms_tick.md。唯一 owner=本 TU，
//      已与宿主 OnlyOrig_Exchange 协调登记。）----
long long get_ms_tick()
{
    return (long long)GlobalData::s_systemTime_.getCurTickCount();
}

namespace exchange_server
{


// ORIG：全局指针经二次解引得到 IExchangeServer 子对象（thread 内嵌子对象地址）。
static IExchangeServer* ES()
{
    // ORIG：mov 0x943d9f4,%eax ; mov (%eax),%eax —— thread 句柄解引到
    // IExchangeServer 视角的内嵌子对象（this+0xc）。
    return (IExchangeServer*)(*(char**)CSession::exchange_server_);
}

// ---------------------------------------------------------------------------
// ctor — ORIG 0x848ffd0 size 0x53：
// 基类构造后清零 +0x851a0/+0x851a4、+0xc 置 1。
// ORIG 栈上临时 CNetwork 构造后立即析构为无操作产物，省略并注释。
// ---------------------------------------------------------------------------
CSession::CSession()
{
    m_triggerEventType = 0;
    m_channelNo = 0;
    m_field0c = 1;
}

// D1 — ORIG 0x8495662 size 0x13，仅调基类析构（隐式空体一致）。
CSession::~CSession()
{
}

int CSession::GetSocket()                       // ORIG 0x8490024 尾调 get_socket
{
    return get_socket();
}

void CSession::SetChannelNo(int channelNo)      // ORIG 0x848ffbe
{
    m_channelNo = (unsigned int)channelNo;
}

int CSession::GetChannelNo()                    // ORIG 0x848ffb0
{
    return (int)m_channelNo;
}

void CSession::SetTriggerSessionEventType(int type)   // ORIG 0x8490038
{
    m_triggerEventType = (unsigned int)type;
}

void CSession::AddTriggerSessionEventType(int type)   // ORIG 0x849004a（|=）
{
    m_triggerEventType |= (unsigned int)type;
}

int CSession::GetTriggerSessionEventType()            // ORIG 0x8490066
{
    return (int)m_triggerEventType;
}

// ---------------------------------------------------------------------------
// Parsing — ORIG 0x8490074 size 0x29a
// ---------------------------------------------------------------------------
int CSession::Parsing(int recvLen)
{
    int parsingLength;                      // -0x24(%ebp)
    unsigned int packetSize;                // -0x14(%ebp)
    int moved;                              // -0xc(%ebp)
    long long tick;                         // -0x20/-0x1c
    char header[HEADER_SIZE];               // -0x41(%ebp)

    parsingLength = (int)m_recvOffset + recvLen;

    tick = get_ms_tick();
    m_lastRecvTick = (unsigned int)tick;                 // +0x04（ORIG 64 位整写，tick 低 4 字节）
    m_recvGapTick = (unsigned int)(tick >> 32);          // +0x08

    if (parsingLength <= 12)
    {
        return 1;
    }

    while (true)                            // 84900be 循环头
    {
        if (m_recvOffset == 0 && parsingLength <= 5)
        {
            // 首次且不足最小头部，等下一次 recv（cmpl $5 → jg 重试）
            return 1;
        }

        if (m_recvOffset == 0)
        {
            packetSize = 0;                 // movl $0,-0x10
        }
        else
        {
            // 缓冲整备：m_recvBuf 回退 m_recvOffset，把已缓存头搬到 m_recv 头部
            m_recvBuf -= m_recvOffset;                        // 2c -= 18
            memcpy(header, (const void*)m_recvBuf, HEADER_SIZE); // memcpy(hdr, buf, 0xd)

            packetSize = *(unsigned int*)(header + 3);        // -0x3e → -0x14（ORIG 实证）
            if (packetSize <= 0xc)
            {
                // 8490111 jbe → 继续下方逻辑（<=12 直接走派发比较）
            }
            else if (packetSize > 0x17318)                    // MAX_SESSION_BUFFER_SIZE
            {
                LogManager::logFormat(1, "ExchangeServerNetwork.h",
                    "DispatchPacket()", 0x18d,
                    "Parsing Packet Size[%d] is Too Large", (int)packetSize);
                return 0;
            }

            if (parsingLength >= (int)packetSize)             // cmp/jb
            {
                bool dispatched = false;
                if (ES() != 0)
                {
                    // slot0 DispatchPacket(this, recvBuf, packetSize)
                    dispatched = ES()->DispatchPacket(
                        this, (char*)m_recvBuf, (int)packetSize);
                }
                moved = -2;                   // -0xc 占位（与 0 判定无关则恒进 trace）
                if (dispatched == false)
                {
                    cMyTrace("DispatchPacket", 0x197, 0)(
                        "DispatchPacket() error line number(%d)", 407);
                }

                parsingLength -= (int)packetSize;
                m_recvBuf += (int)packetSize;
                m_recvOffset = 0;

                if (parsingLength == 0)
                {
                    m_recvBuf = m_recv;       // 回卷读指针
                }

                if (parsingLength > 5)
                {
                    continue;                 // 还有数据→继续解析循环
                }
                break;
            }
            else
            {
                break;                        // jb 8490218：包未收全
            }
        }

        // —— 8490218 起的剩余处理：净收长度异常检查 ——
        if (parsingLength <= 0)
        {
            break;
        }
        if (parsingLength > 0x17318)
        {
            LogManager::logFormat(1, "ExchangeServerNetwork.h",
                "DispatchPacket()", 0x1b7,
                "[PARSING LENGTH EXCEPTION] parsinglength > MAX_SESSION_BUFFER_SIZE , memmove : parsinglength = %d",
                parsingLength);
            return 0;
        }

        try
        {
            memmove(m_recv, m_recvBuf, parsingLength);
            m_recvOffset = parsingLength;
            m_recvBuf = m_recv + parsingLength;
        }
        catch (...)
        {
            LogManager::logFormat(1, "ExchangeServerNetwork.h",
                "DispatchPacket()", 0x1c1,
                "[PARSING EXCEPTION] memmove : parsinglength = %d", parsingLength);
            return 0;
        }

        break;
    }

    return 1;                                 // bx=1 默认返回（8490300/305）
}

// ---------------------------------------------------------------------------
// OnRecv — ORIG 0x849030e size 0x55
// ---------------------------------------------------------------------------
bool CSession::OnRecv()
{
    int ret = recv_packet();
    if (ret > 0)
    {
        return Parsing(ret);                  // xor $1 → true=成功/false=失败
    }
    if (ret < 0)
    {
        return false;                         // eax=0 错误路径
    }
    return true;                              // ret==0 → eax=1
}

// ---------------------------------------------------------------------------
// OnSend — ORIG 0x8490364 size 0x8a
// ---------------------------------------------------------------------------
int CSession::OnSend()
{
    int ret = CNetwork<95000, 450000>::send_packet();
    if (ret < 0)                              // ORIG shr $0x1f + test——仅负值成立
    {
        cMyTrace("OnSend", 0x1e4, 0)("OnSend() Socket[%d] Error(%s)",
            GetSocket(), strerror(errno));
        return -1;
    }
    // ORIG 尾部读 this+0x18(m_recvOffset) 作为返回值（unsigned int → int）。
    return m_recvOffset;
}
// OnError — ORIG 0x84903ee size 0x70
// ---------------------------------------------------------------------------
bool CSession::OnError()
{
    cMyTrace("OnError", 0x1ec, 0)("ExchangeServer OnError() Socket[%d] Error(%s)",
        GetSocket(), strerror(errno));
    return false;
}

// ---------------------------------------------------------------------------
// OnAccept — ORIG 0x849045e size 0x174
// ---------------------------------------------------------------------------
CSession* CSession::OnAccept()
{
    sockaddr_in addr;                         // -0x50(%ebp)，len@-0x54
    socklen_t addrlen = 0x10;

    int fd = accept(GetSocket(), (sockaddr*)&addr, &addrlen);
    if (fd < 0)
    {
        cMyTrace("OnAccept", 0x1f7, 0)("Accept Socket[%d] Error(%s)",
            fd, strerror(errno));
        return 0;                             // mov $0,%eax
    }

    unsigned short port = ntohs(addr.sin_port);
    char* ip = inet_ntoa(addr.sin_addr);
    cMyTrace("session_pool_", 0x1fa, 0)(
        "Accepted other game server ip(%s), port(%d), sock(%d)",
        ip, (int)port, fd);

    // vtable+8 AllocSession()
    CSession* session = (CSession*)ES()->AllocSession();
    if (session != 0)
    {
        session->set_socket(fd, false);
    }
    else
    {
        cMyTrace("session_pool_", 0x202, 0)("session_pool_.Acquire() error");
    }
    return session;                           // ORIG 尾：mov -0xc,%eax
}

// ---------------------------------------------------------------------------
// OnConnect — ORIG 0x84905d2 size 0xfe
// ---------------------------------------------------------------------------
bool CSession::OnConnect()
{
    // vtable+0x10（slot5）GetServerType()；sete 比较返回值==1(EXCHANGE_SERVER_SLAVE)。
    if ((int)ES()->GetServerType() == 1)
    {
        PacketGuard guard;
        guard.clear();
        guard.put_header(1, 0x82);
        guard.put_int(GetServerChannelNo());
        guard.put_int((int)inet_addr(GetServerIP()));
        guard.put_short((int)GetServerPORT());
        guard.finalize(false);          // ORIG 0x8490681：Send 前 finalize，否则 Send 判定未封包不发
        return Send(guard) != 0;        // ORIG 0x8490693：mov %eax,%ebx（Send 返回值）
    }
    return true;                        // ORIG 0x84906c2：mov $1,%ebx
}

// ---------------------------------------------------------------------------
// Send — ORIG 0x84906d0 size 0x97
// ---------------------------------------------------------------------------
int CSession::Send(PacketGuard& guard)
{
    if (guard.is_finallized())
    {
        int len = guard.get_len();
        char* data = guard.get_packet(0);
        int ret = send_packet(data, len);
        if (ret < 0)
        {
            LogManager::logFormat(1, "ExchangeServerNetwork.h",
                "Send(PacketGuard&)", 0x228,
                "Packet.is_finallized return false");
            return 0;
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------
// Listen — ORIG 0x8490767 size 0xc2
// ---------------------------------------------------------------------------
CSession* CSession::Listen(int port, const char* ip)
{
    int sock;                                 // -0x20(%ebp)
    // ORIG：静态调用 CreateListenSocket(&sock, port, ip)（无对象/无 this）；
    // 直接 !返回值 判断（ORIG xor $0x1,%eax; test %al,%al 形态）。
    if (!CNetwork<95000, 450000>::CreateListenSocket(sock, port, ip))
    {
        cMyTrace("CreateListenSocket", 0x236, 0)(
            "CreateListenSocket failed - %s", strerror(errno));
        return 0;
    }

    // vtable+8 AllocSession
    CSession* session = static_cast<CSession*>(ES()->AllocSession());
    if (session != 0)
    {
        session->set_socket(sock, true);
    }
    return session;                            // ORIG mov -0xc,%eax
}

// ---------------------------------------------------------------------------
// Connect — ORIG 0x8490829 size 0xc9
// ---------------------------------------------------------------------------
CSession* CSession::Connect(const char* ip, int port)
{
    int sock;                                 // -0x24(%ebp)
    // CreateConnectionSocket 为静态成员（ORIG 同）；返回非 0=失败码。
    const int ok = CNetwork<95000, 450000>::CreateConnectionSocket(sock, ip, port);

    if (ok != 0)
    {
        cMyTrace("CreateConnectionSocket", 0x24b, 0)(
            "CreateConnectionSocket failed - %s", strerror(errno));
        return 0;
    }

    // vtable+8 AllocSession
    CSession* session = static_cast<CSession*>(ES()->AllocSession());
    if (session != 0)
    {
        session->set_socket(sock, true);
    }
    return session;
}

// ---------------------------------------------------------------------------
// Disconnect — ORIG 0x84908f2 size 0x9f
// ---------------------------------------------------------------------------
void CSession::Disconnect()
{
    cMyTrace("Disconnect", 0x25e, 0)("Disconnect Exchange Server");

    if (CSession::exchange_server_ != 0)
    {
        // vtable+4 UnregisterExchangeServer(channelNo)
        ES()->UnregisterExchangeServer(GetChannelNo());
    }

    disconnect();

    // vtable+12 FreeSession(this)
    ES()->FreeSession(this);
}

}  // namespace exchange_server
