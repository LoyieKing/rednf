// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DbmwClient/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// DbmwClient.cpp —— 按报告 docs/class_func_reports/DbmwClient/*.md 忠实转写。
#include "DbmwClient.h"

#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iostream>   // std::ios_base::Init __ioinit（ORIG GI 键名 _ZN10DbmwClientC2Ev 的前提，P338）
#include "GameTypes.h"     // cMyTrace
#include "LogManager.h"

// ===================== 生命周期 =====================

// ORIG @0x08120454：+0=+1=+2=0、-4=0、+8=-1、new(0x200)、
// +0x1038=0、memset(+0x34,0,0x1001)。
#include "DbmwClient/DbmwClient.inc"
    : m_initialized(false),
      m_connected(false),
      m_sock(-1),
      m_recvBuf(new char[0x200]),
      m_queryLen(0)
{
    memset(m_query, 0, sizeof(m_query));
    m_reconnectCount = 0;
}

// ORIG T _ZN10DbmwClientD1Ev/D2Ev @0x081204be（0x3c）：
// m_connected(+0x02)=0 → close(m_sock(+0x08)) → m_initialized(+0x01)=0 → m_sock=-1
// → operator delete(m_recvBuf(+0x2c))。【Main 补定义：ORIG 4.1 -O0 无条件发射，历轮误删】
#include "DbmwClient/dtor_DbmwClient.inc"
{
    m_connected = false;
    close(m_sock);
    m_initialized = false;
    m_sock = -1;
    // ORIG 0x081204f3：`call _ZdlPv`（operator delete(void*)，非 delete[]）且
    // 调用前**无 test/je 判空**（4.4 对 delete[] 自带判空，多 3 条）⇒ 显式
    // operator delete 调用复现 4.1 直落形（P207）。
    ::operator delete(m_recvBuf);
}

// ORIG T _ZN10DbmwClient9set_queryEPKcz @0x081204fa（0xec）：fmt 空返 false；
// memset(m_query,0,0x1001) → vsprintf → m_queryLen=strlen；越界（>0xfff 或 <=7）
// printf("QueryLength Size ERROR QUERY_SIZE-%d MAX",len) 返 false；否则
// strncasecmp(m_query,"select",6)==0 → m_isSelect=1/0，返 true。
#include "DbmwClient/set_query.inc"
{
    if (fmt == 0)
        return false;
    memset(m_query, 0, 0x1001);
    va_list args;
    va_start(args, fmt);
    vsprintf(m_query, fmt, args);
    va_end(args);
    m_queryLen = strlen(m_query);
    // ORIG 0x08120572/0x08120582：`mov 0x1038(%eax),%eax; cmp $0xfff,%eax; jg`
    // （有符号 jg）⇒ m_queryLen 声明 long（signed）后无需 cast 即出 jg。
    if (m_queryLen > 0xfff || m_queryLen <= 7)
    {
        // ORIG 0x0812059c：格式串含两个 %d 且第二实参为立即数 0x1000
        // （objdump .rodata 实证）；原实现漏了 "MAX_QUERY_SIZE-%d\n" 段。
        printf("QueryLength Size ERROR QUERY_SIZE-%d MAX_QUERY_SIZE-%d\n",
               (int)m_queryLen, 0x1000);
        return false;
    }
    // ORIG 0x081205c1：`movl $"select",(%esp); mov m_query,0x4(%esp)` ⇒ 参数序
    // 是 strncasecmp("select", m_query, 6)；且结果用 jne/两臂 movb 直写
    // （ORIG 0x081205d1/0x081205d9），非 sete 单条物化。
    if (strncasecmp("select", m_query, 6) == 0)
        m_isSelect = 1;
    else
        m_isSelect = 0;
    return true;
}

#include "DbmwClient/Init.inc"
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

#include "DbmwClient/SetServerAddress.inc"
{
    // ORIG @0x08120af6。
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip);
    m_addr.sin_port = htons(port);
}
#include "DbmwClient/SendHeader.inc"
{
    // ORIG @0x08120654：18 字节头 {0x15,type,len+0x12,12B zero} 后 send。
    // ORIG 逐字段直 disp store（total 槽即 header+2）→ 具名视图结构。
    struct __attribute__((packed)) {
        unsigned char b0;
        unsigned char b1;
        int total;
        int z2;
        int z3;
        int z4;
    } header;
    header.b0 = 0x15;
    header.b1 = (unsigned char)type;
    header.total = bodyLen + 0x12;
    header.z2 = 0;
    header.z3 = 0;
    header.z4 = 0;

    ssize_t sent = ::send(m_sock, &header, 0x12, 0);
    if (sent <= 0)
        return false;
    return true;
}

#include "DbmwClient/RegistToMiddleware.inc"
{
    static const char REGIST_GO = 0;  // ORIG _ZZN10DbmwClient18RegistToMiddlewareEvE9REGIST_GO
    int zero = REGIST_GO;             // ORIG 0x81206cc：movl $0x0,-0xc 槽中转（bodyLen 侧）
    if (!SendHeader(REGIST_GO, zero))
        return false;
    return true;
}

#include "DbmwClient/ReceiveRegistAck.inc"
{
    // b28d-L3: DIFF/N82+N89 — ORIG 0x... 帧 0x10c + 成员常量 store 段 vs 我方 0x...
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

    // 2026-09-10（C05② 反向旋钮）：ORIG 0x0812087b 为 `mov -0x30(%ebp),%eax;
    //   cmp -0x2c(%ebp),%eax; je`（具名槽读 + mem 直比），我方 `const int` 被
    //   4.4 常量传播成 `cmpl $0x12,-0x2c(%ebp)` ⇒ 去 const 恢复槽读。
    int expected = 0x12;
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

#include "DbmwClient/Regist.inc"
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

#include "DbmwClient/Connect.inc"
{
    // ORIG @0x081209fa：Init 失败 -98；已连接 -96；
    // connect() -1 且 errno!=EINPROGRESS(0x73) → -97（否则继续 Regist）；
    // Regist 失败 → -95；成功置 connected、返回 0。
    if (Init() != 0)
        return (unsigned int)-98;

    if (m_connected)
        return (unsigned int)-96;

    if (::connect(m_sock, reinterpret_cast<sockaddr*>(&m_addr),
                  sizeof(sockaddr_in)) == -1)
    {
        if (errno == EINPROGRESS)              // ORIG 0x8120a63：cmp $0x73（EINPROGRESS=115）jne -97 块
        {
            if (!Regist())
                return (unsigned int)-95;      // 0xffffffa1
        }
        else
            return (unsigned int)-97;          // 0xffffff9f
    }
    else
    {
        if (!Regist())
            return (unsigned int)-95;          // 0xffffffa1
    }

    m_connected = true;
    return 0;
}

#include "DbmwClient/Close.inc"
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

#include "DbmwClient/ReadData.inc"
{
    // ORIG @0x08120b32：循环 recv 凑满 len；EINTR 重试；EOF 提前返回已读数；
    // 错误记日志返回 -1。
    // ORIG 槽：got@-0x14 < remain@-0x10 < cur@-0xc（4.4 先声明→低槽，
    // 声明序 got→remain→cur）。
    ssize_t got;
    int remain = len;
    char* cur = dst;
    // ORIG 0x08120b32：while (remain > 0) 底部 setg 守卫 + 入口 jmp；
    // recv==0 直接 return len-remain（循环内非 break）。
    while (remain > 0)
    {
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
            return len - (int)remain;
        cur += got;
        remain -= got;
    }
    return len;
}

#include "DbmwClient/DoQuery.inc"
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
    // ORIG 0x08120d0a：`mov 0x1038(%eax),%eax; cmp -0x10(%ebp),%eax`（成员在
    // %eax、栈槽作内存操作数直比）⇒ 字段与 qlen 同型（都 signed 32 位）时
    // 4.4 才出该形。
    if (m_queryLen != qlen)
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

#include "DbmwClient/DoReceive.inc"
{
    // b28d-L3: DIFF/N82+N89 — ORIG 0x... 帧 0x12c + 成员 store 段
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

    // 同上（ORIG 0x08120f3b/0x0812108a 两处 `mov -0x3c; cmp -0x38; je`）。
    int headerLen = 0x12;
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

#include "DbmwClient/Reconnect.inc"
{
    // ORIG @0x08121156。
    ++m_reconnectCount;
    if (Connect() != 0)
    {
        LogManager::logFormat(
            1, "localchina/DbmwClient.cpp", __FUNCTION__, 0x2cd,
            "AccountDBMW Reconnect() Error!! Query(%s)", m_query);
        return false;
    }
    return true;
}

#include "DbmwClient/exec.inc"
{
    // ORIG @0x081211c4：非查询类直接退出；未初始化/未连接先 Reconnect；
    // DoQuery 通过后 DoReceive，0 返回时再按连接状态重试一次。
    if (m_isSelect)
        return 0;

    if (!(IsInitialized() && IsConnected()))
    {
        if (!Reconnect())
            return 0;
    }

    if (!DoQuery())
    {
        if (!(IsInitialized() && IsConnected()))
        {
            LogManager::logFormat(1, "localchina/DbmwClient.cpp", __FUNCTION__,
                                  0x2f4,
                                  "AccountDBMW DoQuery() Error!! Query(%s) line(%d)",
                                  m_query, 0x2f4);
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
        if (!(IsInitialized() && IsConnected()))
        {
            LogManager::logFormat(1, "localchina/DbmwClient.cpp", __FUNCTION__,
                                  0x306,
                                  "AccountDBMW DoReceive() Error!! Query(%s) line(%d)",
                                  m_query, 0x306);
            if (!Reconnect())
                return 0;
        }
        return 0;
    }
    if (received == 1)
        return -2;
    if (received == 2)
        return 1;
}

#include "DbmwClient/getAffectedRowCount.inc"
{
    // ORIG T @0x08120dac：mov 0x30(%eax),%eax; mov %eax,%edx; sar $0x1f,%edx; ret
    // → 读 +0x30（m_affectedRow）符号扩展返 64 位。头声明 long long（非 const）。
    return static_cast<long long>(static_cast<signed int>(m_affectedRow));
}

// IsInitialized/IsConnected 头内联（W）——cpp 不再重复定义。

#include "DbmwClient/Error.inc"
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

    if (useErrno != 0)
    {
        sprintf(buf + strlen(buf), ": %s\n", strerror(savedErrno));
    }
    else
    {
        strcat(buf, "\n");
    }
    fputs(buf, stderr);
    errno = savedErrno;
}
