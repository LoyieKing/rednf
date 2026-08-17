// df_monitor_r — DNFNetworkThread（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
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

#include "DNFNetworkThread.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFUdpHandler.h"
#include "DNFApplication.h"
#include "DNFPacketBuffer.h"
#include "DNFTableBase.h"

CUdpNetworkThread::CUdpNetworkThread()
{
    m_recvQ = 0;
    m_udpHandler = 0;
    m_qLock = 0;
    m_bLock = 0;
}

CUdpNetworkThread::~CUdpNetworkThread()
{
    m_recvQ = 0;
    m_udpHandler = 0;
    m_qLock = 0;
}

void CUdpNetworkThread::attach(CApplication* app)
{
    if (app != 0)
    {
        m_recvQ = app->Get_UdpPacketRecvQ();
        m_udpHandler = app->Get_UdpHandler();
        m_qLock = app->Get_UdpQLock();
        m_bLock = app->Get_UdpBLock();
    }
}

void CUdpNetworkThread::SetUDPQueue(
    std::queue<CUdpRecvBuffer*, std::deque<CUdpRecvBuffer*, std::allocator<CUdpRecvBuffer*> > >* q)
{
    m_recvQ = q;
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0115 | monitor | 与ORIG差异=DIFF | CUdpNetworkThread::dispatch | 详见 function_reports/monitor/_ZN17CUdpNetworkThread8dispatchEPv.md
void CUdpNetworkThread::dispatch(void* param)
{
    if (m_recvQ == 0 || m_udpHandler == 0 || m_qLock == 0)
    {
        throw CDNFException("NetworkThread is Not Ready!\n");
    }
    try
    {
        DNFFLib::Sleep_Ext(5, 0);
        puts("Network Thread Start!");
        m_running = true;
        while (m_running)
        {
            CUdpRecvBuffer* buf;
            {
                CGuard<CMutex> guard((CMutex*)m_bLock);
                buf = new CUdpRecvBuffer;
            }
            int recvSize = 0x1800;
            unsigned short srcPort = 0;
            unsigned int fromAddr = 0;
            if (m_udpHandler->RecvFromClient((char*)buf, &recvSize, &fromAddr, &srcPort) == 1)
            {
                unsigned int recvByte = (unsigned int)recvSize;
                CUdpRecvBuffer* pkt = buf;
                if (((PacketHeader*)buf)->packetSize == recvSize)
                {
                    if (((PacketHeader*)buf)->packetSize < 0x1800)
                    {
                        if (recvSize < 0x1801)
                        {
                            ((PacketHeader*)buf)->m_connNo = fromAddr;
                            ((PacketHeader*)buf)->reversed1 = srcPort;
                            {
                                CGuard<CMutex> guard((CMutex*)m_qLock);
                                ((std::queue<CUdpRecvBuffer*>*)m_recvQ)->push(pkt);
                            }
                        }
                        else
                        {
                            DNF_LOG_SCOPE_LINE(0x85,"./log/recvErr",
                                "Recv Byte is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                                ((PacketHeader*)buf)->packetSize, recvByte, *(unsigned short*)buf);
                            {
                                CGuard<CMutex> guard((CMutex*)m_bLock);
                                delete buf;
                            }
                        }
                    }
                    else
                    {
                        DNF_LOG_SCOPE_LINE(0x79,"./log/recvErr",
                            "Packet Size is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                            ((PacketHeader*)buf)->packetSize, recvByte, *(unsigned short*)buf);
                        {
                            CGuard<CMutex> guard((CMutex*)m_bLock);
                            delete buf;
                        }
                    }
                }
                else
                {
                    DNF_LOG_SCOPE_LINE(0x6e,"./log/recvErr",
                        "Packet Size is Incorrect! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                        ((PacketHeader*)buf)->packetSize, recvByte, *(unsigned short*)buf);
                    {
                        CGuard<CMutex> guard((CMutex*)m_bLock);
                        delete buf;
                    }
                }
            }
            else
            {
                {
                    CGuard<CMutex> guard((CMutex*)m_bLock);
                    delete buf;
                }
            }
        }
        return;
    }
    catch (CDNFException& e)
    {
        printf("CUdpNetworkThread::dispatch() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd : %s\n", e.what());
        throw CDNFException("CUdpNetworkThread::dispatch() Recv  Socket Exception Break!");
    }
    catch (...)
    {
        puts("CUdpNetworkThread::dispatch() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd");
        throw CDNFException("CUdpNetworkThread::dispatch() Recv  Socket Exception Break!");
    }
}
