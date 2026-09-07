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
// 全局 exchange_server_E(0x943d9f4, B) 在 ORIG 直接承载 IExchangeServer*：
// 五函数切片实证全部虚调用均为「全局指针值直作 this、*P 作 vptr」单层解引
// （Listen 0x84907e0 / Connect 0x84908a9 / OnConnect 0x84905da /
//   Parsing 0x849016a / OnAccept），无二次解引层。
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


// ORIG：虚调用点全部内联装载全局（mov 0x943d9f4,%eax），无 helper call。
// 用宏保持 -O0 内联形（P49：static 函数 -O0 生真实 call，与 ORIG 不符）。
#define ES() ((IExchangeServer*)CSession::exchange_server_)

// ---------------------------------------------------------------------------
// ctor — ORIG 0x848ffd0 size 0x53：
// 基类构造后清零 +0x851a0/+0x851a4、+0xc 置 1。
// ORIG 基类构造后先在栈上建一个局部 CNetwork 临时并立即析构（lea -0x851a8;
// C1;D1，对 *this 无副作用）。该临时对可观察行为无任何影响，且重建引入后 4.4
// 工具链会为具备非平凡析构的临时额外生成 EH landing pad（push esi/ebx +
// _Unwind_Resume），反而比 ORIG 4.1 的 20 条多出 15 条指令，因此按 N 族跨版本
// 结构性差异省略（语义等价，字节层 N 封顶）。
// [b22判词·N] ORIG 0x848ffd0 基类 ctor 后于栈帧内建匿名 CNetwork<95000,450000>
// 临时（lea -0x851a8;C1;C1;D1）并因此撑出 sub $0x851b8 巨型帧；该临时对 *this
// 无可观察副作用，且 4.4 工具链会为非平凡析构临时额外出 EH landing pad，复现
// 反而指令膨胀。+0xc 写 movb $0x1 低字节（m_field0c 为基类 int，仅低字节被用作
// 标志），4.4 对 char* 取址成员写产 add+movb 而非 ORIG 的 disp(%eax) 单条——属
// 跨版本寻址伪影。三者均不可源码复现，语义等价（帧/临时/寻址 N 封顶）。
// ---------------------------------------------------------------------------
CSession::CSession()
{
    m_triggerEventType = 0;
    m_channelNo = 0;
    *(char*)&m_field0c = 1;   // ORIG movb $0x1,0xc(%eax)（仅写低字节）
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
                    cMyTrace(__PRETTY_FUNCTION__, 0x197, 0)(
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
        // ORIG 0x849033a：`call Parsing; xor $1,%eax; test %al,%al; je <mov$1>`
        // —— P194 X1 块序：register asm(eax) + ^=1 失败分支，返 0；否则返 1。
        register unsigned char parsed asm("eax") = (unsigned char)Parsing(ret);
        parsed ^= 1;
        if (parsed)
            return false;                     // 失败分支（Parsing 返回 0）
        return true;                          // 成功（Parsing 返回 1）
    }
    if (ret < 0)
    {
        return false;                          // eax=0 错误路径
    }
    return true;                              // ret==0 → eax=1
}

// ---------------------------------------------------------------------------
// OnSend — ORIG 0x8490364 size 0x8a
// ---------------------------------------------------------------------------
int CSession::OnSend()
{
    // ORIG 0x8490364：call send_packet 后 shr $0x1f,%eax 直测符号位，无局部槽
    // （P0 槽-局部律：寄存器直传 ⇒ 内联右值）。
    if (CNetwork<95000, 450000>::send_packet() < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e4, 0)("OnSend() Socket[%d] Error(%s)",
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
    cMyTrace(__PRETTY_FUNCTION__, 0x1ec, 0)("ExchangeServer OnError() Socket[%d] Error(%s)",
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
        cMyTrace(__PRETTY_FUNCTION__, 0x1f7, 0)("Accept Socket[%d] Error(%s)",
            fd, strerror(errno));
        return 0;                             // mov $0,%eax
    }

    unsigned short port = ntohs(addr.sin_port);
    char* ip = inet_ntoa(addr.sin_addr);
    cMyTrace(__PRETTY_FUNCTION__, 0x1fa, 0)(
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
        cMyTrace(__PRETTY_FUNCTION__, 0x202, 0)("session_pool_.Acquire() error");
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
        guard.put_short(GetServerPORT());
        guard.finalize(false);          // ORIG 0x8490681：Send 前 finalize，否则 Send 判定未封包不发
        // ORIG 0x8490698：mov %eax,%ebx 跨 dtor 存活后 mov %ebx,%eax 透传（bool→bool，
        // 无 test/setne 规范化——Send 真实返回 bool，PRETTY_FUNCTION 串实证）。
        return Send(guard);
    }
    return true;                        // ORIG 0x84906c2：mov $1,%ebx
}

// ---------------------------------------------------------------------------
// Send — ORIG 0x84906d0 size 0x97
// ---------------------------------------------------------------------------
bool CSession::Send(PacketGuard& guard)
{
    // ORIG 0x84906d0：test %al,%al + je（正向 if，logFormat 块为 je 目标后置，
    // P32：!f() 会生 xor $1 物化而 ORIG 无）。send_packet 失败路径独立 mov$0;jmp
    // 出口不经 logFormat。
    if (guard.is_finallized())
    {
        // ORIG 0x84906f1：get_len 结果升格 %ebx 跨 get_packet 调用存活（P52），
        // get_packet 结果进 eax 直接入参槽不落栈槽——单语句右到左实参求值形。
        int ret = send_packet(guard.get_packet(0), guard.get_len());
        if (ret < 0)
        {
            return 0;
        }
    }
    else
    {
        LogManager::logFormat(1, "ExchangeServerNetwork.h",
            "Send(PacketGuard&)", 0x228,
            "Packet.is_finallized return false");
        return 0;
    }
    return 1;                                 // ORIG L1: mov $1（块在 logFormat 后）
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
        cMyTrace(__PRETTY_FUNCTION__, 0x236, 0)(
            "CreateListenSocket failed - %s", strerror(errno));
        return 0;
    }

    // vtable+8 AllocSession
    CSession* session = static_cast<CSession*>(ES()->AllocSession());
    if (session != 0)
    {
        session->set_socket(sock, true);
        return session;                       // ORIG mov -0xc,%eax + jmp 岛
    }
    return 0;                                  // ORIG 0x849081e: mov $0,%eax（je 目标后置块）
}

// ---------------------------------------------------------------------------
// Connect — ORIG 0x8490829 size 0xc9
// ---------------------------------------------------------------------------
CSession* CSession::Connect(const char* ip, int port)
{
    int sock;                                 // -0x24(%ebp)
    // CreateConnectionSocket 为静态成员（ORIG 同）；返回非 0=失败码。
    const int ok = CNetwork<95000, 450000>::CreateConnectionSocket(sock, ip, port);

    // ORIG 0x849084c：cmpl 槽后 setne %al + test %al,%al + je（P194 sete 物化形，
    // 4.4 对槽比零恒折直跳，须 register bool(al) 强制物化）。
    register bool failed asm("eax") = (ok != 0);
    if (failed)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24b, 0)(
            "CreateConnectionSocket failed - %s", strerror(errno));
        return 0;
    }

    // vtable+8 AllocSession
    CSession* session = static_cast<CSession*>(ES()->AllocSession());
    if (session != 0)
    {
        session->set_socket(sock, true);
        return session;
    }
    return 0;                                  // ORIG 0x84908e7: mov $0,%eax
}

// ---------------------------------------------------------------------------
// Disconnect — ORIG 0x84908f2 size 0x9f
// ---------------------------------------------------------------------------
void CSession::Disconnect()
{
    cMyTrace(__PRETTY_FUNCTION__, 0x25e, 0)("Disconnect Exchange Server");

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
