// df_guild_r — DNFTcpNetworkThread (split from source/guild per ORIG layout)
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFTcpNetworkThread.h"
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
#include "DNFNetworkThread.h"
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
                CTcpHandler* handler = (CTcpHandler*)m_handler;
                CPeer* p = (CPeer*)handler->GetEventPtr(i);
                bool isIn = p != 0 && handler->IsSetInEvent(i);
                if (isIn && p->RecvPacket() != 1)
                {
                    p->DisConnSig();
                    m_net->DeletePeer(p);
                    p = 0;
                }
                bool isOut = p != 0 && p->get_remain_sendlen() != 0 &&
                             handler->IsSetOutEvent(i);
                if (isOut && p->get_remain_sendlen() < 0x1801)
                {
                    p->send_packet();
                }
                handler->IsSetErrEvent(i);
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

void CTcpNetworkThread::attach(CTcpNetSystem* net)
{
    if (net != 0)
    {
        m_net = net;
        m_recvQ =
            ((CSwapQueue<std::queue<CTcpRecvBuffer*>, 2>*)net->Get_TcpSwapQPacket())->GetRecvQ();
        m_handler = net->Get_TcpHandler();
        m_recvQLock = net->Get_TcpRecvQLock();
        m_recvBLock = net->Get_TcpRecvBLock();
        m_sendQ = net->Get_TcpSendQPacket();
        m_sendQLock = net->Get_TcpSendQLock();
        m_sendBLock = net->Get_TcpSendBLock();
    }
}
