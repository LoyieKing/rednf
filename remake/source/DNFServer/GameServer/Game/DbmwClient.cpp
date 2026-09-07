// DbmwClient.cpp —— 按报告 docs/class_func_reports/DbmwClient/*.md 忠实转写。
#include "DbmwClient.h"

#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "GameTypes.h"     // cMyTrace
#include "LogManager.h"

// ===================== 生命周期 =====================

// ORIG @0x08120454：+0=+1=+2=0、-4=0、+8=-1、new(0x200)、
// +0x1038=0、memset(+0x34,0,0x1001)。
DbmwClient::DbmwClient()
    : m_isSelect(false),
      m_initialized(false),
      m_connected(false),
      m_pad03(0),
      m_reconnectCount(0),
      m_sock(-1),
      m_timeout(),
      m_recvBuf(0),
      m_affectedRow(0),
      m_queryLen(0)
{
    memset(m_field0c, 0, sizeof(m_field0c));
    m_recvBuf = new char[0x200];
    memset(m_query, 0, sizeof(m_query));
}

unsigned int DbmwClient::Init()
{
    // ORIG @0x081205e6：未初始化时 socket(AF_INET=2, SOCK_STREAM=1, 0)；
    // 成功后清 +0x0c/+0x10、置 +0x01=1；sock 失败返回 0xffffff9c。
    if (m_initialized)
        return 0;

    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock == (int)0xffffffff)
        return (unsigned int)-100;

    m_field0c[0] = 0;
    m_field0c[1] = 0;
    m_initialized = true;
    return 0;
}

void DbmwClient::SetServerAddress(const char* ip, unsigned short port)
{
    // ORIG @0x08120af6。
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip);
    m_addr.sin_port = htons(port);
}
bool DbmwClient::SendHeader(char type, int bodyLen)
{
    // ORIG @0x08120654：18 字节头 {0x15,type,len+0x12,12B zero} 后 send。
    unsigned char header[0x12];
    header[0] = 0x15;
    header[1] = (unsigned char)type;
    int total = bodyLen + 0x12;
    *reinterpret_cast<int*>(&header[2]) = total;
    *reinterpret_cast<int*>(&header[6]) = 0;
    *reinterpret_cast<int*>(&header[10]) = 0;
    *reinterpret_cast<int*>(&header[14]) = 0;

    ssize_t sent = ::send(m_sock, header, sizeof(header), 0);
    return sent > 0;
}

bool DbmwClient::RegistToMiddleware()
{
    static const char REGIST_GO = 0;  // ORIG _ZZN10DbmwClient18RegistToMiddlewareEvE9REGIST_GO
    return SendHeader(REGIST_GO, 0);
}

bool DbmwClient::ReceiveRegistAck()
{
    // ORIG @0x08120702：select(tv_sec=2) → 读 0x12 头 → msg_no==0 成功。
    // N78 判词：ORIG fd_set 清零为 bmp[32] int 槽循环（无 rep stos），4.4-O0 的
    // FD_ZERO/FD_SET 恒内联成 rep stos 展开（P116/P168 平台墙），语义等价豁免。
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_sock, &readfds);

    int ret = select(m_sock + 1, &readfds, 0, 0, &tv);
    if (ret < 1)
    {
        int errNo = errno;
        cMyTrace trace(__FUNCTION__, 0xfd, 0);
        trace("AccountDBMW select error : %d(%s)", errNo, strerror(errNo));
        Close();
        return false;
    }

    const int expected = 0x12;
    int got = ReadData(m_recvBuf, expected);
    if (got == expected)
    {
        int msgNo = (int)(char)m_recvBuf[1];
        {
            cMyTrace trace(__FUNCTION__, 0x117, 0);
            trace("AccountDBMW read msg_no : %d", msgNo);
        }
        if (msgNo == 0)
        {
            cMyTrace trace(__FUNCTION__, 0x11d, 0);
            trace("AccountDBMW regist Ok");
            return true;
        }
        Close();
        LogManager::logFormat(1, "localchina/DbmwClient.cpp",
                              __FUNCTION__, 0x123,
                              "AccountDBMW regist fail");
        return false;
    }
    else
    {
        if (got != -1)
        {
            cMyTrace trace(__FUNCTION__, 0x110, 0);
            trace("ReceiveRegistAck read size error : %d, expected : %d",
                  got, expected);
        }
        Close();
        return false;
    }
}

bool DbmwClient::Regist()
{
    // ORIG @0x081209ba 整读（range 0x081209ba..0x081209f9，63 字节）：
    //   +0x06 mov this,(%esp); call RegistToMiddleware
    //   +0x11 xor $0x1,%eax; test %al,%al; je +0x1f        ← 成功跳下一段
    //   +0x18 mov $0x0,%eax; jmp +0x3d(end)                 ← 失败返回 0
    //   +0x1f mov this,(%esp); call ReceiveRegistAck
    //   +0x2a xor $0x1,%eax; test %al,%al; je +0x38
    //   +0x31 mov $0x0,%eax; jmp +0x3d
    //   +0x38 mov $0x1,%eax
    //   +0x3d leave; ret
    // 关键：原写法 `if (A()) return B();` 把 B 的结果**透传**（`call B; leave; ret`），
    // ORIG 实际是**双臂各自 xor 取反 + 常量返回 + 单一 epilogue**（gap 缺的 7 条
    // 就是第二个 xor/test/je/mov/jmp 段）。故必须写成两次"取反判断 + 常量返回"，
    // 并用 goto 把两个失败出口汇到同一个 `mov $0x0,%eax; jmp end`（技巧 23）。
    // 技巧 1（在册，比 X1 register 形更短）：被调本身返回 **bool** 时，
    // `if (!boolcall())` 在 4.4 直接产 ORIG 形 `call; xor $0x1,%eax;
    // test %al,%al; je <下一段>` + `mov $0x0,%eax; jmp end` 落点岛，
    // 无需 register asm 绑定（X1 形是给 int 返回准备的，这里用反而把
    // 分支极性做成 jne，实测 ndiff 9→11 变差）。
    if (!RegistToMiddleware())
        return false;
    if (!ReceiveRegistAck())
        return false;
    return true;
}

unsigned int DbmwClient::Connect()
{
    // ORIG @0x081209fa：Init 失败 -98；已连接 -96；
    // connect() -1 且 errno!=EINPROGRESS(0x73) → -97（否则继续 Regist）；
    // Regist 失败 → -95；成功置 connected、返回 0。
    if (Init() != 0)
        return (unsigned int)-98;

    if (m_connected)
        return (unsigned int)-96;

    sockaddr* addr = reinterpret_cast<sockaddr*>(&m_addr);
    if (::connect(m_sock, addr, sizeof(sockaddr_in)) == -1)
    {
        if (errno != EINPROGRESS)              // ORIG 0x8120a63：cmp $0x73（EINPROGRESS=115）
            return (unsigned int)-97;          // 0xffffff9f
        if (!Regist())
            return (unsigned int)-95;          // 0xffffffa1
    }
    else
    {
        if (!Regist())
            return (unsigned int)-95;          // 0xffffffa1
    }

    m_connected = true;
    return 0;
}

int DbmwClient::Close()
{
    // ORIG @0x08120ab0：`call close; cmp $0xffffffff,%eax; sete %al;
    // test %al,%al; je else` —— **结果不落栈**，直接在 %eax 上比较并 sete 归一。
    // 技巧 9（`(bool)(intcall()==K)` → cmp+sete+test）：引入 `int result` 具名
    // 局部会让 4.4 先 `mov %eax,-0xc(%ebp)` 再 `cmpl $-1,-0xc`（形态失配 4 条、
    // 且帧从 0x18 涨到 0x28）。免局部直比同时解决两处。
    if ((bool)(::close(m_sock) == -1))
        return errno;

    m_connected = false;
    m_initialized = false;
    m_sock = -1;
    return 0;
}

int DbmwClient::ReadData(char* dst, int len)
{
    // ORIG @0x08120b32：循环 recv 凑满 len；EINTR 重试；EOF 提前返回已读数；
    // 错误记日志返回 -1。
    // ORIG 槽：got@-0x14 < remain@-0x10 < cur@-0xc（4.4 先声明→低槽，
    // 声明序 got→remain→cur）。
    ssize_t got;
    size_t remain = (size_t)len;
    char* cur = dst;
    // ORIG 0x08120b32：外 while(true) + 顶 `if ((int)remain<1) return len`；
    // recv==0 break 后 return len-remain（底部 setg 守卫 + 入口 jmp 布局）。
    while (true)
    {
        if ((int)remain < 1)
            return len;
        got = ::recv(m_sock, cur, remain, 0);
        if (got < 0)
        {
            if (errno == EINTR)
                continue;
            // ORIG 0x08120b32：errno 在各使用点独立宏展开（__errno_location
            // 多调，%ebx 缓存 ptr 跨 strerror call）——errNo 命名缓存形少 2 条。
            LogManager::logFormat(
                1, "localchina/DbmwClient.cpp", __FUNCTION__, 0x1dd,
                "AccountDBMW read error; recv result:%d, errno:%d:%s",
                (int)got, errno, strerror(errno));
            return -1;
        }
        if (got == 0)
            break;
        cur += got;
        remain -= (size_t)got;
    }
    return len - (int)remain;
}

bool DbmwClient::DoQuery()
{
    static const char UPDATE_QUERY_GO = 1;  // ORIG _ZZN10DbmwClient7DoQueryEvE15UPDATE_QUERY_GO
    // ORIG @0x08120cd0。
    m_affectedRow = 0;
    // 技巧 5（signedness 装载序定律）：ORIG 是 `mov %eax,-0x10(%ebp)` 之后
    // **两条内存-立即数直比** `cmpl $0xfff,-0x10(%ebp); jg` +
    // `cmpl $0x7,-0x10(%ebp); jg`。用 `(long)qlen` 会迫使 4.4 先把槽值装进
    // %eax 再 `cmp`（实测多 1 条 mov、2 条 cmp 形态失配）。
    // ⇒ 局部必须是 **int**（ORIG 槽宽 4B、比较用有符号 jg），且比较写成
    //   `> 0xfff` 而非 `>= 0x1000`，让立即数与 ORIG 完全一致。
    int qlen = (int)strlen(m_query);
    if (qlen > 0xfff || qlen <= 7)
        return false;
    // 技巧 5/27：ORIG 是 `mov 0x1038(%eax),%eax; cmp -0x10(%ebp),%eax`
    // （成员先载入 %eax、再与栈槽**内存操作数**直比）。`m_queryLen` 是
    // `unsigned long`、`qlen` 是 `int`，signedness 失配迫使 4.4 把两侧都装进
    // 寄存器再比（多一条 mov）。显式把 qlen 转成同型即恢复单内存操作数形。
    if ((unsigned long)qlen != m_queryLen)
        return false;

    if (!SendHeader(UPDATE_QUERY_GO, (int)m_queryLen))
    {
        Close();
        return false;
    }
    ssize_t sent = ::send(m_sock, m_query, m_queryLen, 0);
    if (sent < 1)
    {
        Close();
        return false;
    }
    return true;
}

int DbmwClient::DoReceive()
{
    // ORIG @0x08120dbc：select → 读 18B 头 → msg_no 分支：
    //   1 = 服务端断开 ack（Close 后返 1）；payloadLen<0x1b 时读 payload，
    //   payload 前 4 字节写入 affectedRow。
    // N78 判词：ORIG readfds 清零/置位为 bmp[32] int 槽循环（无 rep stos），
    // 4.4-O0 的 FD_ZERO/FD_SET 恒内联成 rep stos（P116/P168 平台墙），语义等价豁免；
    // 非符号位域 shr$0x5 读取同以语义等价口径收。
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_sock, &readfds);

    int ret = select(m_sock + 1, &readfds, 0, 0, &tv);
    if (ret < 1)
    {
        int errNo = errno;
        cMyTrace trace(__FUNCTION__, 0x25e, 0);
        trace("AccountDBMW select error : %d(%s)", errNo, strerror(errNo));
        Close();
        return 0;
    }

    const int headerLen = 0x12;
    int got = ReadData(m_recvBuf, headerLen);
    if (got != headerLen)
    {
        if (got != -1)
        {
            cMyTrace trace(__FUNCTION__, 0x271, 0);
            trace("AccountDBMW header read size error : %d, expected : %d",
                  got, headerLen);
        }
        Close();
        return 0;
    }

    int msgNo = (int)(char)m_recvBuf[1];
    {
        cMyTrace trace(__FUNCTION__, 0x278, 0);
        trace("AccountDBMW read msg_no : %d", msgNo);
    }
    if (msgNo == 1)
    {
        Close();
        return 1;
    }

    unsigned int payloadLen =
        *reinterpret_cast<unsigned int*>(m_recvBuf + 2);
    if (payloadLen >= 0x1b)
    {
        Close();
        LogManager::logFormat(1, "localchina/DbmwClient.cpp", __FUNCTION__,
                              0x284,
                              "AccountDBMW header packet size invalid!!");
        return 0;
    }

    int payload = (int)payloadLen - 0x12;
    int payloadGot = ReadData(m_recvBuf + 0x12, payload);
    if (payload != payloadGot)
    {
        if (payloadGot != -1)
        {
            cMyTrace trace(__FUNCTION__, 0x290, 0);
            trace("AccountDBMW payload read size error:%d", payloadGot);
        }
        Close();
        return 0;
    }

    {
        cMyTrace trace(__FUNCTION__, 0x296, 0);
        trace("AccountDBMW payload read ok");
    }
    memcpy(&m_affectedRow, m_recvBuf + 0x16, sizeof(unsigned int));
    return 2;
}

bool DbmwClient::Reconnect()
{
    // ORIG @0x08121156。
    ++m_reconnectCount;
    bool ok = (Connect() == 0);
    if (!ok)
    {
        LogManager::logFormat(
            1, "localchina/DbmwClient.cpp", __FUNCTION__, 0x2cd,
            "AccountDBMW Reconnect() Error!! Query(%s)", m_query);
    }
    return ok;
}

int DbmwClient::exec()
{
    // ORIG @0x081211c4：非查询类直接退出；未初始化/未连接先 Reconnect；
    // DoQuery 通过后 DoReceive，0 返回时再按连接状态重试一次。
    if (m_isSelect)
        return 0;

    bool needReconnect = !(IsInitialized() && IsConnected());
    if (needReconnect && !Reconnect())
        return 0;

    if (!DoQuery())
    {
        needReconnect = !(IsInitialized() && IsConnected());
        if (needReconnect)
        {
            LogManager::logFormat(1, "localchina/DbmwClient.cpp", __FUNCTION__,
                                  0x2f4,
                                  "AccountDBMW exec() reconnect fail Query(%s)",
                                  m_query);
            if (!Reconnect())
                return -1;
            if (!DoQuery())
                return -1;
        }
        return 0;
    }

    int received = DoReceive();
    if (received == 0)
    {
        needReconnect = !(IsInitialized() && IsConnected());
        if (needReconnect)
        {
            LogManager::logFormat(1, "localchina/DbmwClient.cpp", __FUNCTION__,
                                  0x306,
                                  "AccountDBMW exec() reconnect fail Query(%s)",
                                  m_query);
            if (!Reconnect())
                return 0;
        }
    }
    return received;
}

long long DbmwClient::getAffectedRowCount()
{
    // ORIG T @0x08120dac：mov 0x30(%eax),%eax; mov %eax,%edx; sar $0x1f,%edx; ret
    // → 读 +0x30（m_affectedRow）符号扩展返 64 位。头声明 long long（非 const）。
    return static_cast<long long>(static_cast<signed int>(m_affectedRow));
}

// IsInitialized/IsConnected 头内联（W）——cpp 不再重复定义。

void DbmwClient::Error(int level, int useErrno, char* fmt, ...)
{
    // ORIG @0x08120c18：捕获 errno → vsprintf → 可选追加 ": strerror\n"
    // → fputs(stderr) → 还原 errno。level 仅占位（ORIG 未消费）。
    (void)level;
    char buf[200];
    va_list args;
    va_start(args, fmt);
    int savedErrno = errno;
    vsprintf(buf, fmt, args);
    va_end(args);

    if (useErrno == 0)
    {
        strcat(buf, "\n");
    }
    else
    {
        sprintf(buf + strlen(buf), ": %s\n", strerror(savedErrno));
    }
    fputs(buf, stderr);
    errno = savedErrno;
}
