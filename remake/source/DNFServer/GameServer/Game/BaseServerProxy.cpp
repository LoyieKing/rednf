// df_game_r 还原 —— BaseServerProxy（E1 批次）。
// 参照 docs/class_func_reports/BaseServerProxy.md。
#include "BaseServerProxy.h"

#include <errno.h>
#include <string.h>

#include "GameBase.h"

BaseServerProxy::BaseServerProxy(char* ip, int port, char* name)
    : m_ip(ip), m_port(port), m_serverName(name)
{
    m_socketState = ENUM_SOCKET_STATE_DISCONNECTED;
}

BaseServerProxy::~BaseServerProxy()
{
}

void BaseServerProxy::OnDisconnect()
{
}

bool BaseServerProxy::Init()
{
    return InitTcp(true);
}

bool BaseServerProxy::InitTcp(bool bSetNonBlock)
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
                cMyTrace("bool BaseServerProxy::InitTcp(bool)", 0x134, 0)
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
                cMyTrace("bool BaseServerProxy::InitTcp(bool)", 0x13d, 0)
                    ("%s CreateConnectionSocket() error(%d:%d:%s)",
                     GetServerName().c_str(), ret, *__errno_location(),
                     strerror(*__errno_location()));
                return 0;
            }
        }
    }
    return 1;
}

void BaseServerProxy::Disconnect()
{
    cMyTrace("virtual void BaseServerProxy::Disconnect()", 0x148, 0)
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

bool BaseServerProxy::SendTcpPacket(char* data, int len)
{
    if (m_socketState != ENUM_SOCKET_STATE_CONNECTED)
    {
        cMyTrace("bool BaseServerProxy::SendTcpPacket(char*, int)", 0x15d, 0)
            ("cannot send packet because %s is disconnected", GetServerName().c_str());
        return 0;
    }
    if (m_tcpHandler.send_packet(data, len) < 0)
    {
        return 0;
    }
    return 1;
}

bool BaseServerProxy::OnTcpRecv()
{
    int recvLen = m_tcpHandler.recv_packet();
    if (recvLen > 0)
    {
        if (!Parsing(recvLen))
        {
            cMyTrace("bool BaseServerProxy::OnTcpRecv()", 0x16f, 0)
                ("%s occur Error at parsing receive packet", GetServerName().c_str());
            Disconnect();
            return 0;
        }
        return 1;
    }
    if (recvLen < 0)
    {
        LogManager::logFormat(1, "BaseServerProxy.cpp", "bool BaseServerProxy::OnTcpRecv()",
                              0x17a, "%s maybe Die!!!", GetServerName().c_str());
        Disconnect();
        return 0;
    }
    return 1;
}

int BaseServerProxy::OnTcpSend()
{
    int sendRet = m_tcpHandler.send_packet();
    if (sendRet < 0)
    {
        cMyTrace("int BaseServerProxy::OnTcpSend()", 0x18f, 0)
            ("%s OnSend() Socket[%d] Error(%s)", GetServerName().c_str(),
             GetTcpHandler()->get_socket(), strerror(*__errno_location()));
        cMyTrace("int BaseServerProxy::OnTcpSend()", 0x192, 0)
            ("Disconnect to %s (send_packet() returns minus value)", GetServerName().c_str());
        Disconnect();
        return -1;
    }
    return m_tcpHandler.m_recvOffset;
}

bool BaseServerProxy::OnTcpError()
{
    if (*__errno_location() != 0)
    {
        cMyTrace("bool BaseServerProxy::OnTcpError()", 0x19f, 0)
            ("%s OnError() Socket[%d] Error(%s)", GetServerName().c_str(),
             GetTcpHandler()->get_socket(), strerror(*__errno_location()));
        return 0;
    }
    return 1;
}
