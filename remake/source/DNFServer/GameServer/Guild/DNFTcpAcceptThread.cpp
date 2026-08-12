// df_guild_r — DNFTcpAcceptThread (split from source/guild per ORIG layout)
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFTcpAcceptThread.h"
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

CTcpAcceptThread::CTcpAcceptThread()
    : m_net(0), m_recvQLock(0), m_recvBLock(0)
{
}

CTcpAcceptThread::~CTcpAcceptThread()
{
    m_recvQLock = 0;
    m_net = 0;
}

void CTcpAcceptThread::dispatch(void* param)
{
    try
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
        m_running = true;
        DNFFLib::Sleep_Ext(5, 0);
        while (m_running)
        {
            if (!m_sock.pollReadEvent())
            {
                continue;
            }
            CPeer* peer = m_net->CreatePeer();
            if (m_sock.accept(*peer->GetTcpSocket()) != 1)
            {
                printf("Accept GameServer Fail(Port : %d)\n", peer->GetTcpSocket()->getHandle());
            }
            printf("Accept GameServer(Port : %d)\n", peer->GetTcpSocket()->getHandle());
            peer->InitPeer(
                ((CSwapQueue<std::queue<CTcpRecvBuffer*>, 2>*)m_net->Get_TcpSwapQPacket())->GetRecvQ(),
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
