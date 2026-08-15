// df_monitor_r — DNFTcpAcceptThread（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "DNFTcpAcceptThread.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFPacketBuffer.h"
#include "DNFTableBase.h"
#include "Peer.h"
#include "TcpNetSystem.h"

CTcpAcceptThread::CTcpAcceptThread()
    : m_net(0), m_recvQLock(0), m_recvBLock(0)
{
}

CTcpAcceptThread::~CTcpAcceptThread()
{
    m_recvQLock = 0;
    m_net = 0;
}

void CTcpAcceptThread::attach(CTcpNetSystem* net)
{
    if (net != 0)
    {
        m_net = net;
        m_recvQLock = net->Get_TcpRecvQLock();
        m_recvBLock = net->Get_TcpRecvBLock();
        m_port = net->Get_TcpServerPort();
    }
}

void CTcpAcceptThread::dispatch(void* param)
{
    try
    {
        if (!m_sock.open())
        {
            printf("Tcp Accept Socket Open Err");
        }
        else if (!m_sock.bind(m_port, true))
        {
            printf("Tcp Accept Socket Bind Err");
        }
        else if (!m_sock.listen(5))
        {
            printf("Tcp Accept Socket Listen Err");
        }
        else
        {
            m_running = true;
            DNFFLib::Sleep_Ext(5, 0);
            while (m_running)
            {
                if (m_sock.pollReadEvent())
                {
                    CPeer* peer = m_net->CreatePeer();
                    TCPSocket* sock = peer->GetTcpSocket();
                    if (m_sock.accept(*sock) != 1)
                    {
                        printf("Accept GameServer Fail(Port : %d)\n", sock->getHandle());
                    }
                    printf("Accept GameServer(Port : %d)\n", sock->getHandle());
                    CMutex* recvB = m_net->Get_TcpRecvBLock();
                    CMutex* recvQ = m_net->Get_TcpRecvQLock();
                    void* q = m_net->Get_TcpSwapQPacket()->GetRecvQ();
                    peer->InitPeer(
                        (std::queue<CTcpRecvBuffer*>*)q, recvQ, recvB);
                    peer->ConnSig();
                    m_net->InsertAcceptedPeer(peer);
                }
            }
        }
    }
    catch (CDNFException& e)
    {
        printf("CTcpAcceptThread::dispatch() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd : %s\n", e.what());
        throw CDNFException("CTcpAcceptThread::dispatch() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd!");
    }
    catch (...)
    {
        puts("CTcpAcceptThread::dispatch() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd");
        throw CDNFException("CTcpAcceptThread::dispatch() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd!");
    }
}
