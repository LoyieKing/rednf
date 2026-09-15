// df_dbmw_r - DNFTcpNetworkThread (ORIG DNFTcpNetworkThread.cpp)
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

CTcpNetworkThread::CTcpNetworkThread()
{
    m_net = 0;
    m_recvQ = 0;
    m_handler = 0;
    m_recvQLock = 0;
    m_recvBLock = 0;
    m_sendQ = 0;
    m_sendQLock = 0;
    m_sendBLock = 0;
    m_runningFlag = 0;
}

CTcpNetworkThread::~CTcpNetworkThread()
{
    m_recvQ = 0;
    m_handler = 0;
    m_recvQLock = 0;
    m_net = 0;
    m_sendQ = 0;
    m_sendQLock = 0;
    m_sendBLock = 0;
}

void CTcpNetworkThread::attach(CTcpNetSystem* net)
{
    if (net)
    {
        m_net = net;
        m_recvQ = net->Get_TcpSwapQPacket()->GetRecvQ();
        m_handler = net->Get_TcpHandler();
        m_recvQLock = net->Get_TcpRecvQLock();
        m_recvBLock = net->Get_TcpRecvBLock();
        m_sendQ = net->Get_TcpSendQPacket();
        m_sendQLock = net->Get_TcpSendQLock();
        m_sendBLock = net->Get_TcpSendBLock();
    }
}
// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-DBM-DIFF-0110 已消除）
void CTcpNetworkThread::dispatch(void* param)
{
    CPeer* peer = 0;
    int nEvent = 0;
    m_runningFlag = 1;
    DNFFLib::Sleep_Ext(5, 0);
    try
    {
        while (m_runningFlag)
        {
            errno = 0;
            DNFFLib::Sleep_Ext(0, 5);
            if (!m_net)
                continue;
            m_net->SetEpollAcceptedPeers();
            m_net->SendPacket();
            nEvent = m_net->WaitForEvent();
            if (nEvent == 0)
                continue;
            if (nEvent < 0)
            {
                if (errno != 4)
                {
                    if (errno != 0)
                        return;
                }
            }
            for (int i = 0; i < nEvent; i++)
            {
                peer = (CPeer*)m_handler->GetEventPtr(i);
                if (peer != 0 && m_handler->IsSetInEvent(i))
                {
                    if (peer->RecvPacket() != 1)
                    {
                        peer->DisConnSig();
                        m_net->DeletePeer(peer);
                        peer = 0;
                    }
                }
                if (peer != 0 && peer->get_remain_sendlen() != 0 &&
                    m_handler->IsSetOutEvent(i))
                {
                    if ((unsigned int)peer->get_remain_sendlen() > 0x1800)
                    {
                    }
                    else
                    {
                        peer->send_packet();
                    }
                }
                m_handler->IsSetErrEvent(i);
            }
        }
        CMyFileLog log(__FUNCTION__, 0xae);
        log("./log/TcpRecv", "RecvThread Terminate");
    }
    catch (CDNFException& e)
    {
        printf("CTcpNetworkThread::dispatch() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd : %s\n", e.what());
        throw CDNFException("CTcpNetworkThread::dispatch() Recv  Socket Exception Break!");
    }
    catch (...)
    {
        puts("CTcpNetworkThread::dispatch() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd");
        throw CDNFException("CTcpNetworkThread::dispatch() Recv  Socket Exception Break!");
    }
}
