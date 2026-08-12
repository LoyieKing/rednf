// df_dbmw_r - DNFTcpAcceptThread (ORIG DNFTcpAcceptThread.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

CTcpAcceptThread::CTcpAcceptThread() : m_net(0), m_recvQLock(0), m_recvBLock(0) {}

CTcpAcceptThread::~CTcpAcceptThread()
{
    m_recvQLock = 0;
    m_net = 0;
}

void CTcpAcceptThread::attach(CTcpNetSystem* net)
{
    if (net)
    {
        m_net = net;
        m_recvQLock = net->Get_TcpRecvQLock();
        m_recvBLock = net->Get_TcpRecvBLock();
        m_port = net->Get_TcpServerPort();
    }
}
void CTcpAcceptThread::dispatch(void* param)
{
    if (!m_sock.open())
    {
        printf("Tcp Accept Socket Open Err");
        return;
    }
    if (!m_sock.bind(m_port, true))
    {
        printf("Tcp Accept Socket Bind Err");
        return;
    }
    if (!m_sock.listen(5))
    {
        printf("Tcp Accept Socket Listen Err");
        return;
    }
    m_stop = 1;
    DNFFLib::Sleep_Ext(5, 0);
    try
    {
        while (m_stop)
        {
            if (!m_sock.pollReadEvent())
                continue;
            CPeer* peer = m_net->CreatePeer();
            if (!m_sock.accept(*peer->GetTcpSocket()))
                printf("Accept GameServer Fail(Port : %d)\n",
                       peer->GetTcpSocket()->getHandle());
            printf("Accept GameServer(Port : %d)\n",
                   peer->GetTcpSocket()->getHandle());
            peer->InitPeer(m_net->Get_TcpSwapQPacket()->GetRecvQ(),
                           m_net->Get_TcpRecvQLock(), m_net->Get_TcpRecvBLock());
            peer->ConnSig();
            m_net->InsertAcceptedPeer(peer);
        }
    }
    catch (CDNFException& e)
    {
        printf("CTcpNetworkThread::dispatch() Except Break : %s\n", e.what());
        throw CDNFException("CTcpNetworkThread::dispatch() Recv  Socket Exception Break!");
    }
    catch (...)
    {
        puts("CTcpNetworkThread::dispatch() Except Break");
        throw CDNFException("CTcpNetworkThread::dispatch() Recv  Socket Exception Break!");
    }
}
