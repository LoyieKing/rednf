// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BaseServerProxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// df_game_r 还原 —— BaseServerProxy（E1 批次）。
// 参照 docs/class_func_reports/BaseServerProxy.md。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN15BaseServerProxyC2EPciS0_`（ORIG 0x82fdd4f）；ctor 已是首强符号。
#include <iostream>

#include "BaseServerProxy.h"

#include <errno.h>
#include <string.h>

#include "GameBase.h"

// ============================================================================
// [b28d-55] 本 TU 判词表（verdict = game_func_report --no-compile，fresh .o）：
//   InitTcp 0x082fd47c  DIFF(171/167) P167 CLEAR(25/25)；本 TU 经
//     dnf_game_cxx.sh 路由 C6446R（4.4.6 后端，N7 敏感族翻正）；残差 =
//     errnoVal/ret 寄存器互换（路由表注释已记，P230 N7 边界）⇒ N 封顶。
// ============================================================================

#include "BaseServerProxy/BaseServerProxy.inc"
    : m_ip(ip), m_port(port), m_serverName(name)
{
    m_socketState = ENUM_SOCKET_STATE_DISCONNECTED;
}

#include "BaseServerProxy/dtor_BaseServerProxy.inc"
{
}

#include "BaseServerProxy/OnDisconnect.inc"
{
}

#include "BaseServerProxy/Init.inc"
{
    return InitTcp(true);
}

#include "BaseServerProxy/InitTcp.inc"
{
    if (m_ip.length() == 0 || m_port == 0)
    {
        return 0;
    }
    if (m_tcpHandler.m_sock < 0)
    {
        int ret;
        if (bSetNonBlock)
        {
            if ((ret = m_tcpHandler.CreateConnectionSocket(m_tcpHandler.m_sock, m_ip.c_str(),
                                                           m_port)) != 0)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x134, 0)
                    ("%s CreateConnectionSocket() error(%d:%d:%s)",
                     GetServerName().c_str(), ret, *__errno_location(),
                     strerror(*__errno_location()));
                return 0;
            }
        }
        else
        {
            if ((ret = m_tcpHandler.CreateConnectionBlockSocket(m_tcpHandler.m_sock,
                                                                m_ip.c_str(), m_port)) != 0)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x13d, 0)
                    ("%s CreateConnectionSocket() error(%d:%d:%s)",
                     GetServerName().c_str(), ret, *__errno_location(),
                     strerror(*__errno_location()));
                return 0;
            }
        }
    }
    return 1;
}

#include "BaseServerProxy/Disconnect.inc"
{
    cMyTrace(__PRETTY_FUNCTION__, 0x148, 0)
        ("BaseServerProxy::Disconnect(), ServerName:%s,SocketState:%d,IP:%s,Port:%d,Socket:%d",
         GetServerName().c_str(), m_socketState, m_ip.c_str(), m_port,
         m_tcpHandler.get_socket());
    if (m_ip.length() == 0 || m_port == 0)
    {
        return;
    }
    m_tcpHandler.disconnect();
    m_socketState = ENUM_SOCKET_STATE_DISCONNECTED;
    OnDisconnect();
}

#include "BaseServerProxy/SendTcpPacket.inc"
{
    if (m_socketState != ENUM_SOCKET_STATE_CONNECTED)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x15d, 0)
            ("cannot send packet because %s is disconnected", GetServerName().c_str());
        return 0;
    }
    if (m_tcpHandler.send_packet(data, len) < 0)
    {
        return 0;
    }
    return 1;
}

#include "BaseServerProxy/OnTcpRecv.inc"
{
    int recvLen = m_tcpHandler.recv_packet();
    if (recvLen > 0)
    {
        if (!Parsing(recvLen))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x16f, 0)
                ("%s occur Error at parsing receive packet", GetServerName().c_str());
            Disconnect();
            return 0;
        }
        return 1;
    }
    if (recvLen < 0)
    {
        LogManager::logFormat(1, "BaseServerProxy.cpp", __PRETTY_FUNCTION__,
                              0x17a, "%s maybe Die!!!", GetServerName().c_str());
        Disconnect();
        return 0;
    }
    return 1;
}

#include "BaseServerProxy/OnTcpSend.inc"
{
    int sendRet = m_tcpHandler.send_packet();
    if (sendRet < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x18f, 0)
            ("%s OnSend() Socket[%d] Error(%s)", GetServerName().c_str(),
             GetTcpHandler()->get_socket(), strerror(*__errno_location()));
        cMyTrace(__PRETTY_FUNCTION__, 0x192, 0)
            ("Disconnect to %s (send_packet() returns minus value)", GetServerName().c_str());
        Disconnect();
        return -1;
    }
    return m_tcpHandler.m_recvOffset;
}

#include "BaseServerProxy/OnTcpError.inc"
{
    if (*__errno_location() != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x19f, 0)
            ("%s OnError() Socket[%d] Error(%s)", GetServerName().c_str(),
             GetTcpHandler()->get_socket(), strerror(*__errno_location()));
        return 0;
    }
    return 1;
}
