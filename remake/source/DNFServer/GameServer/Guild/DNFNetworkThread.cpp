// df_guild_r — DNFNetworkThread (split from source/guild per ORIG layout)
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFNetworkThread.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

CUdpNetworkThread::CUdpNetworkThread()
{
    m_queue = 0;
    m_handler = 0;
    m_lock = 0;
    m_bLock = 0;
}

CUdpNetworkThread::~CUdpNetworkThread()
{
    m_queue = 0;
    m_handler = 0;
    m_lock = 0;
}

void CUdpNetworkThread::dispatch(void* param)
{
    try
    {
        if (m_queue == 0 || m_handler == 0 || m_lock == 0)
        {
            throw CDNFException("NetworkThread is Not Ready!\n");
        }
        DNFFLib::Sleep_Ext(5, 0);
        puts("Network Thread Start!");
        m_running = true;
        while (m_running)
        {
            CUdpRecvBuffer* buf;
            {
                CGuard<CMutex> g((CMutex*)m_bLock);
                buf = (CUdpRecvBuffer*)CUdpRecvBuffer::operator new(0x1804);
            }
            int len = 0x1800;
            unsigned short port = 0;
            unsigned int ip = 0;
            char ok = ((CUdpHandler*)m_handler)->RecvFromClient((char*)buf, &len, &ip, &port);
            unsigned int recvByte = (unsigned int)len;
            if (ok == 1)
            {
                CUdpRecvBuffer* buf2 = buf;
                if (((PacketHeader*)buf2)->packetSize != len)
                {
                    DNF_LOG_SCOPE_LINE(0x66,"./log/recvErr",
                        "Packet Size is Incorrect! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                        ((PacketHeader*)buf2)->packetSize, recvByte,
                        ((PacketHeader*)buf2)->packetId);
                    {
                        CGuard<CMutex> g((CMutex*)m_bLock);
                        CUdpRecvBuffer::operator delete(buf2);
                    }
                }
                else
                {
                    if (((PacketHeader*)buf2)->packetSize < 0x1800)
                    {
                        if (len >= 0x1801)
                        {
                            DNF_LOG_SCOPE_LINE(0x7d,"./log/recvErr",
                                "Recv Byte is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                                ((PacketHeader*)buf2)->packetSize, recvByte,
                                ((PacketHeader*)buf2)->packetId);
                            {
                                CGuard<CMutex> g((CMutex*)m_bLock);
                                CUdpRecvBuffer::operator delete(buf2);
                            }
                        }
                        else
                        {
                            ((PacketHeader*)buf2)->reversed2 = ip;
                            ((PacketHeader*)buf2)->reversed1 = port;
                            {
                                CGuard<CMutex> g((CMutex*)m_lock);
                                ((std::queue<CUdpRecvBuffer*>*)m_queue)->push(buf2);
                                unsigned int qsize =
                                    ((std::queue<CUdpRecvBuffer*>*)m_queue)->size();
                            }
                        }
                    }
                    else
                    {
                        DNF_LOG_SCOPE_LINE(0x71,"./log/recvErr",
                            "Packet Size is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                            ((PacketHeader*)buf2)->packetSize, recvByte,
                            ((PacketHeader*)buf2)->packetId);
                        {
                            CGuard<CMutex> g((CMutex*)m_bLock);
                            CUdpRecvBuffer::operator delete(buf2);
                        }
                    }
                }
            }
            else
            {
                {
                    CGuard<CMutex> g((CMutex*)m_bLock);
                    CUdpRecvBuffer::operator delete(buf);
                }
            }
        }
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

void CUdpNetworkThread::attach(CApplication* app)
{
    if (app != 0)
    {
        m_queue = app->Get_UdpPacketRecvQ();
        m_handler = app->Get_UdpHandler();
        m_lock = app->Get_UdpQLock();
        m_bLock = app->Get_UdpBLock();
    }
}

void CUdpNetworkThread::SetUDPQueue(std::queue<CUdpRecvBuffer*>* q)
{
    m_queue = q;
}
