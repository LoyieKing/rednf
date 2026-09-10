// df_game_r 还原 —— CUdpHandler（UDP 句柄，ORIG 0x8483518 族，8 函数）。
// 参照 docs/class_func_reports/CUdpHandler.md；getErrno 同 TU（ORIG 0x8483e75）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN11CUdpHandlerC2Ev`（ORIG 0x8483e59）；键名符号需前移（首定义现为 getErrno）。
#include <iostream>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "CUdpHandler.h"
#include "GameTypes.h"

int getErrno()
{
    return errno;
}

CUdpHandler::CUdpHandler()
{
    m_serverSocket = -1;
    m_clientSocket = -1;
}

unsigned int CUdpHandler::InetAddr(const char* ip) const
{
    return inet_addr(ip);
}

int CUdpHandler::InitClientSocket()
{
    timeval rcvtimeo;
    int err;
    m_clientSocket = socket(AF_INET, SOCK_DGRAM, 0x11);
    if (m_clientSocket == -1)
    {
        err = getErrno();
        printf("Could not create a UDP socket : %d\n", err);
        return -1;
    }
    rcvtimeo.tv_sec = 10;
    rcvtimeo.tv_usec = 0;
    setsockopt(m_clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&rcvtimeo,
               sizeof(rcvtimeo));
    return m_clientSocket;
}

int CUdpHandler::InitServerSocket(int port)
{
    int err;
    sockaddr_in addr;
    timeval rcvtimeo;
    m_serverSocket = socket(AF_INET, SOCK_DGRAM, 0x11);
    if (m_serverSocket == -1)
    {
        err = getErrno();
        printf("Could not create a UDP socket : %d\n", err);
        return -1;
    }
    memset((char*)&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons((unsigned short)port);
    if (bind(m_serverSocket, (sockaddr*)&addr, 0x10) != 0)
    {
        err = getErrno();
        switch (err)
        {
        case 0x62:
            printf("Port %d for receiving UDP is in use\n", port);
            break;
        case 99:
            puts("Cannot assign requested address");
            break;
        // b28c-L10：ORIG 0x848362b 决策树尾 `test %eax,%eax; je 0x11f`（跳过 default 体）
        // ⇒ 原码有 case 0（空体，err==0 不打印）。补上后 4.4 复用 %eax 生成同一
        // test/je 尾，且 default 体不再需要 `if (err != 0)` 重载槽判断（消除
        // `cmpl $0x0,-0xc(%ebp); je` 伪差）。case 书写序照 ORIG 树序 0x62→99→0。
        case 0:
            break;
        default:
            printf("Could not bind UDP receive port. Error= %d , strerror = %s\n",
                   err, strerror(err));
            break;
        }
        m_serverSocket = -1;
    }
    rcvtimeo.tv_sec = 10;
    rcvtimeo.tv_usec = 0;
    setsockopt(m_serverSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&rcvtimeo,
               sizeof(rcvtimeo));
    return m_serverSocket;
}

bool CUdpHandler::RecvFromClient(char* buf, int* size, unsigned int* addr,
                                unsigned short* port) const
{
    // b28d-solo-CUdpHandler 判词（AE）：ORIG 0x8483732 在 len=0x10 后立即
    // `lea -0x20(%ebp),%ebx` 物化第 6 实参 &len（4.1 右起求值）；4.4 对直传 `&len`
    // 与 P226 语句表达式恒延后到压栈点。复现形 = 纯 register 指针局部
    // `register socklen_t* plen = &len;`（P231/P248：无 asm 绑定、合法存储类提示，
    // 值留 %ebx 免槽且装载点提前）⇒ 逐条同形（clean 口径 AE；p167 调用点 8=8 CLEAR）。
    // 帧 0x44、槽 -0x20/-0x1c/-0xc 与 ORIG 相同。
    int err;
    sockaddr_in from;
    socklen_t len;
    if (m_serverSocket == -1)
    {
        return 0;
    }
    len = 0x10;
    // b28c-L10：ORIG 在 len=0x10 后立即 `lea -0x20(%ebp),%ebx` 物化 &len 跨 recvfrom
    // 调用（P226 裸语句表达式升格 callee-saved）；直传实参被 4.4 延后到压栈点。
    register socklen_t* plen = &len;
    *size = recvfrom(m_serverSocket, buf, *size, 0, (sockaddr*)&from,
                     plen);
    if (*size == -1)
    {
        err = getErrno();
        switch (err)
        {
        case 0x58:
            puts("Error fd not a socket");
            break;
        case 0x68:
            puts("Error connection reset - host not reachable");
            break;
        default:
            printf("Hm! Time out Or Socket Error = %d\n", err);
            break;
        }
        return 0;
    }
    if (*size < 1)
    {
        printf("Socket closed? Recv size = %d\n", *size);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *addr = ntohl(from.sin_addr.s_addr);
    buf[*size] = '\0';
    return 1;
}

bool CUdpHandler::RecvFromServer(char* buf, int* size, unsigned int* addr,
                                unsigned short* port) const
{
    // b28d-solo-CUdpHandler 判词（AE）：同 RecvFromClient —— ORIG 0x8483a52 先把
    // &len 物化到 %ebx（P231 纯 register 指针局部形复现，无 asm 绑定），
    // 4.4 直传 `&len`/语句表达式恒延后；现逐条同形（clean 口径 AE，p167 8=8 CLEAR）。
    int err;
    sockaddr_in from;
    socklen_t len;
    if (m_clientSocket == -1)
    {
        return 0;
    }
    len = 0x10;
    register socklen_t* plen = &len;
    *size = recvfrom(m_clientSocket, buf, *size, 0, (sockaddr*)&from,
                     plen);
    if (*size == -1)
    {
        err = getErrno();
        switch (err)
        {
        case 0x58:
            puts("Error fd not a socket");
            break;
        case 0x68:
            puts("Error connection reset - host not reachable");
            break;
        default:
            printf("Hm! Time out Or Socket Error = %d\n", err);
            break;
        }
        return 0;
    }
    if (*size < 1)
    {
        printf("Socket closed? Recv size = %d\n", *size);
        return 0;
    }
    *port = ntohs(from.sin_port);
    *addr = ntohl(from.sin_addr.s_addr);
    buf[*size] = '\0';
    return 1;
}

bool CUdpHandler::SendToClient(char* buf, int len, unsigned short port,
                              const char* ip, unsigned int addr) const
{
    int sent;
    int err;
    if (m_clientSocket == -1)
    {
        cMyTrace trace(__FUNCTION__, 0xe1, 5);
        trace("[%s][%d]", __FUNCTION__, 0xe1);
        return 0;
    }
    if (ip == 0 && addr == 0)
    {
        return 0;
    }
    if (ip != 0)
    {
        addr = inet_addr(ip);
    }
    if (m_serverSocket == -1)
    {
        return 0;
    }
    if (port == 0)
    {
        if (addr == 0)
        {
            sent = send(m_serverSocket, buf, len, 0);
        }
    }
    else
    {
        sockaddr_in to;
        memset(&to, 0, 0x10);
        to.sin_family = AF_INET;
        to.sin_port = htons(port);
        to.sin_addr.s_addr = ntohl(addr);
        sent = sendto(m_serverSocket, buf, len, 0, (sockaddr*)&to, 0x10);
    }
    if (sent == -1)
    {
        err = getErrno();
        switch (err)
        {
        case 0x6f:
        case 0x70:
        case 0x71:
            printf("Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            break;
        case 0x61:
            puts("err EAFNOSUPPORT in send");
            break;
        default:
            printf("err = %d , strerror = %s in send\n", err, strerror(err));
            break;
        }
        return 0;
    }
    if (sent == 0)
    {
        puts("no data sent in send");
        return 0;
    }
    if (len != sent)
    {
        printf("Only %s out of %d bytes sent\n", sent, len);
        return 0;
    }
    else
    {
        return 1;
    }
}

bool CUdpHandler::SendToServer(char* buf, int len, unsigned short port,
                              const char* ip) const
{
    sockaddr_in to;
    int sent;
    int err;
    if (m_clientSocket == -1)
    {
        // b28c-L10：P221 全表达式临时链——ORIG 槽序 0x194→-0x70 最深，4.4 具名对象
        // 反装为 -0x30 最浅；临时链复现 ORIG 槽深方向。
        cMyTrace(__FUNCTION__, 0x194, 5)("[%s][%d]", __FUNCTION__, 0x194);
        return 0;
    }
    else if (m_clientSocket == -1)
    {
        return 0;
    }
    if (port == 0)
    {
        if (ip == 0)
        {
            sent = send(m_clientSocket, buf, len, 0);
        }
    }
    else
    {
        // b28c-L10：P68 memset 首参 cast 触发 lea 先行（ORIG `lea -0x80(%ebp),%eax`
        // 在 `movl $0x10,0x8(%esp)` 之前；无 cast 时 4.4 右到左先压常量）。
        memset((char*)&to, 0, 0x10);
        to.sin_family = AF_INET;
        to.sin_port = htons(port);
        to.sin_addr.s_addr = inet_addr(ip);
        sent = sendto(m_clientSocket, buf, len, 0, (sockaddr*)&to, 0x10);
    }
    if (sent == -1)
    {
        err = getErrno();
        switch (err)
        {
        case 0x6f:
        case 0x70:
        case 0x71:
        {
            cMyTrace(__FUNCTION__, 0x1ca, 5)
                ("CUdpHandler::SendToServer Error: EHOSTUNREACH");
            break;
        }
        case 0x61:
        {
            cMyTrace(__FUNCTION__, 0x1d1, 5)
                ("CUdpHandler::SendToServer Error: EAFNOSUPPORT");
            break;
        }
        default:
        {
            cMyTrace(__FUNCTION__, 0x1d8, 5)
                ("CUdpHandler::SendToServer Error : other err");
            break;
        }
        }
        return 0;
    }
    if (sent == 0)
    {
        cMyTrace(__FUNCTION__, 0x1e2, 5)("CUdpHandler::SendToServer Error: 0 == s");
        return 0;
    }
    if (len != sent)
    {
        cMyTrace(__FUNCTION__, 0x1ea, 5)("CUdpHandler::SendToServer Error : len != s");
        return 0;
    }
    else
    {
        return 1;
    }
}
