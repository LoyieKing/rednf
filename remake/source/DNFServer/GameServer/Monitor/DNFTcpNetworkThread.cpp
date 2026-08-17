// df_monitor_r — DNFTcpNetworkThread（从 MonitorTypes/App/Table 拆分）
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

#include "DNFTcpNetworkThread.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFTableBase.h"
#include "DNFTcpHandler.h"
#include "Peer.h"
#include "TcpNetSystem.h"

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
    if (net != 0)
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

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0114 | monitor | 与ORIG差异=DIFF | CTcpNetworkThread::dispatch | 详见 function_reports/monitor/_ZN17CTcpNetworkThread8dispatchEPv.md
void CTcpNetworkThread::dispatch(void* param)
{
    CPeer* peer = 0;
    int eventCount = 0;
    m_runningFlag = 1;
    DNFFLib::Sleep_Ext(5, 0);
    try
    {
        while (m_runningFlag)
        {
            errno = 0;
            DNFFLib::Sleep_Ext(0, 5);
            if (m_net == 0)
            {
                continue;
            }
            m_net->SetEpollAcceptedPeers();
            m_net->SendPacket();
            eventCount = m_net->WaitForEvent();
            if (eventCount == 0)
            {
                continue;
            }
            if (eventCount < 0 && errno != EINTR && errno != 0)
            {
                break;
            }
            for (int i = 0; i < eventCount; i++)
            {
                peer = (CPeer*)((CTcpHandler*)m_handler)->GetEventPtr(i);
                if (peer != 0 && ((CTcpHandler*)m_handler)->IsSetInEvent(i))
                {
                    if (peer->RecvPacket() < 1)
                    {
                        peer->DisConnSig();
                        m_net->DeletePeer(peer);
                        peer = 0;
                    }
                }
                if (peer != 0 && peer->get_remain_sendlen() != 0 &&
                    ((CTcpHandler*)m_handler)->IsSetOutEvent(i))
                {
                    if (peer->get_remain_sendlen() < 0x1801)
                    {
                        peer->send_packet();
                    }
                }
                ((CTcpHandler*)m_handler)->IsSetErrEvent(i);
            }
        }
        DNF_LOG_SCOPE_LINE(0xae, "./log/TcpRecv", "RecvThread Terminate");
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
