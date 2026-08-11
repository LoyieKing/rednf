// df_guild_r — DNFTcpHandler (split from source/guild per ORIG layout)
#include <string.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include "tinyxml.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel_Next.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel_Next.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFTcpHandler.h"
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

EpollHandler::EpollHandler()
{
    Init();
}

EpollHandler::~EpollHandler()
{
    Destroy();
}

void EpollHandler::Destroy()
{
    delete[] m_events;
    m_events = 0;
}

int EpollHandler::Init()
{
    m_epollFd = epoll_create(1000);
    if (m_epollFd < 0)
    {
        puts("[Epoll::init] Can't init epoll create");
        return 0;
    }
    m_events = new epoll_event[1000];
    if (m_events == 0)
    {
        printf("[Epoll::init] Can't alloc event memory");
        return 0;
    }
    return 1;
}

int EpollHandler::SetEpoll(void* ptr, int fd, bool flag)
{
    m_field4 = flag ? 0x8000001d : 0x1d;
    m_ptr = ptr;
    CGuard<CMutex> g(&m_mutex);
    epoll_event ev;
    ev.events = (unsigned int)m_field4;
    ev.data.ptr = ptr;
    int r = epoll_ctl(m_epollFd, 1, fd, &ev);
    return r < 0 ? errno : 0;
}

int EpollHandler::ResetEpoll(int fd)
{
    memset((char*)this, 0, 0xc);
    *(int*)((char*)this + 4) = 1;
    CGuard<CMutex> g(&m_mutex);
    epoll_event ev;
    ev.events = 1;
    ev.data.ptr = m_ptr;
    int r = epoll_ctl(m_epollFd, 2, fd, &ev);
    return r < 0 ? errno : 0;
}

int EpollHandler::WaitForEvent()
{
    return epoll_wait(GetEpollFD(), (epoll_event*)GetEpollEvents(), 1000, 100);
}

bool EpollHandler::IsSetErrEvent(int idx)
{
    return (m_events[idx].events & 0x18) != 0;
}

bool EpollHandler::IsSetOutEvent(int idx)
{
    return (m_events[idx].events & 4) != 0;
}

unsigned int EpollHandler::IsSetInEvent(int idx)
{
    return m_events[idx].events & 1;
}

void* EpollHandler::GetEventPtr(int idx)
{
    return m_events[idx].data.ptr;
}

CTcpHandler::CTcpHandler()
{
    m_epoll = new EpollHandler;
}

CTcpHandler::~CTcpHandler()
{
    if (m_epoll != 0)
    {
        delete m_epoll;
        m_epoll = 0;
    }
}

int CTcpHandler::SetPeer(void* ptr, int fd, bool flag)
{
    if (m_epoll == 0)
    {
        return -1;
    }
    return m_epoll->SetEpoll(ptr, fd, flag);
}

int CTcpHandler::ResetEpoll(int fd)
{
    if (m_epoll == 0)
    {
        return -1;
    }
    int result = m_epoll->ResetEpoll(fd);
    return result;
}

int CTcpHandler::WaitForEvent()
{
    if (m_epoll == 0)
    {
        return -1;
    }
    return m_epoll->WaitForEvent();
}

bool CTcpHandler::IsSetErrEvent(int idx)
{
    if (m_epoll == 0)
    {
        return 0;
    }
    return m_epoll->IsSetErrEvent(idx);
}

bool CTcpHandler::IsSetOutEvent(int idx)
{
    if (m_epoll == 0)
    {
        return 0;
    }
    return m_epoll->IsSetOutEvent(idx);
}

unsigned int CTcpHandler::IsSetInEvent(int idx)
{
    if (m_epoll == 0)
    {
        return 0;
    }
    return m_epoll->IsSetInEvent(idx);
}

void* CTcpHandler::GetEventPtr(int idx)
{
    if (m_epoll == 0)
    {
        return 0;
    }
    return m_epoll->GetEventPtr(idx);
}

CProtocol::~CProtocol()
{
}

int EpollHandler::GetEpollFD()
{
    return m_epollFd;
}

void* EpollHandler::GetEpollEvents()
{
    return (void*)m_events;
}

CProtocol::CProtocol()
{
}
