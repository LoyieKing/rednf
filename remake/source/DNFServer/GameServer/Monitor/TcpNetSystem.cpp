// df_monitor_r — TcpNetSystem（从 MonitorTypes/App/Table 拆分）
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

#include "TcpNetSystem.h"
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
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "Peer.h"

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
    if (m_events != 0)
    {
        ::operator delete[](m_events);
    }
    m_events = 0;
}

int EpollHandler::Init()
{
    m_epollFd = epoll_create(1000);
    if (m_epollFd < 0)
    {
        puts("[Epoll::init] Can\'t init epoll create");
        return 0;
    }
    m_events = new epoll_event[1000];
    if (m_events == 0)
    {
        printf("[Epoll::init] Can\'t alloc event memory");
        return 0;
    }
    return 1;
}

int EpollHandler::SetEpoll(void* peer, int fd, bool flag)
{
    if (flag)
    {
        m_event.events = 0x8000001d;
    }
    else
    {
        m_event.events = 0x1d;
    }
    m_event.data.ptr = peer;
    CGuard<CMutex> guard(&m_mutex);
    register int r;
    if (epoll_ctl(m_epollFd, 1, fd, &m_event) < 0)
    {
        r = errno;
    }
    else
    {
        r = 0;
    }
    return r;
}

int EpollHandler::ResetEpoll(int fd)
{
    memset(&m_event, 0, 0xc);
    m_event.events = 1;
    CGuard<CMutex> guard(&m_mutex);
    register int r;
    if (epoll_ctl(m_epollFd, 2, fd, &m_event) < 0)
    {
        r = errno;
    }
    else
    {
        r = 0;
    }
    return r;
}

int EpollHandler::WaitForEvent()
{
    epoll_event* events = (epoll_event*)GetEpollEvents();
    int fd = GetEpollFD();
    return epoll_wait(fd, events, 1000, 100);
}

char EpollHandler::IsSetErrEvent(int idx)
{
    return (char)((m_events[idx].events & 0x18) != 0);
}

char EpollHandler::IsSetInEvent(int idx)
{
    return (char)((m_events[idx].events & 1) != 0);
}

char EpollHandler::IsSetOutEvent(int idx)
{
    return (char)((m_events[idx].events & 4) != 0);
}

void* EpollHandler::GetEventPtr(int idx)
{
    return m_events[idx].data.ptr;
}

CTcpNetSystem::CTcpNetSystem()
    : m_handler(0), m_networkThread(0), m_acceptThread(0), m_port(0)
{
}

CTcpNetSystem::~CTcpNetSystem()
{
    CleanPeers();
    if (m_handler != 0)
    {
        delete m_handler;
        m_handler = 0;
    }
    if (m_acceptThread != 0)
    {
        m_acceptThread->stop();
        delete m_acceptThread;
        m_acceptThread = 0;
    }
    if (m_networkThread != 0)
    {
        m_networkThread->stop();
        delete m_networkThread;
        m_networkThread = 0;
    }
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0021 | monitor | 与ORIG差异=DIFF | CTcpNetSystem::Init | 详见 function_reports/monitor/_ZN13CTcpNetSystem4InitEt.md
void CTcpNetSystem::Init(unsigned short port)
{
    m_port = port;
    m_handler = new CTcpHandler;
    m_acceptThread = new CTcpAcceptThread;
    m_acceptThread->attach(this);
    if (!m_acceptThread->begin())
    {
        throw;
    }
    m_networkThread = new CTcpNetworkThread;
    m_networkThread->attach(this);
    if (!m_networkThread->begin())
    {
        throw;
    }
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0019 | monitor | 与ORIG差异=DIFF | CTcpNetSystem::OpenTcpService | 详见 function_reports/monitor/_ZN13CTcpNetSystem14OpenTcpServiceERiPKct.md
bool CTcpNetSystem::OpenTcpService(int& sockRef, const char* ip, unsigned short port)
{
    CPeer* peer = CreatePeer();
    TCPSocket* sock = peer->GetTcpSocket();
    if (!sock->open())
    {
        puts("tcpSock.open() Fail!");
        DNF_LOG_SCOPE_LINE(0x118, "./log/TcpServer", "tcpSock.open() Fail!");
        DeletePeer(peer);
        return 0;
    }
    if (!sock->connect(ip, port))
    {
        puts("tcpSock.connect Fail!");
        DNF_LOG_SCOPE_LINE(0x123, "./log/TcpServer", "tcpSock.connect(%s, %d) Fail!", ip, (unsigned int)port);
        DeletePeer(peer);
        return 0;
    }
    sock->setOptNonBlock();
    CMutex* b = Get_TcpRecvBLock();
    CMutex* q = Get_TcpRecvQLock();
    void* recvQ = Get_TcpSwapQPacket()->GetRecvQ();
    peer->InitPeer((std::queue<CTcpRecvBuffer*>*)recvQ, q, b);
    peer->ConnSig();
    SetEpollConnectedPeer(peer);
    sockRef = sock->getHandle();
    return 1;
}

void CTcpNetSystem::CleanPeers()
{
    std::map<unsigned int, CPeer*>::iterator it = m_peers.begin();
    while (it != m_peers.end())
    {
        CGuard<CMutex> guard(&m_mutex78);
        register CPeer* p = it->second;
        delete p;
        ++it;
    }
    m_peers.clear();
}

void CTcpNetSystem::SetEpollAcceptedPeers()
{
    CGuard<CMutex> guard(&m_mutex60);
    if (m_peerQ.empty())
    {
        return;
    }
    CPeer* peer = 0;
    while (!m_peerQ.empty())
    {
        peer = m_peerQ.front();
        int result = 0;
        if ((result = m_handler->SetPeer(peer, peer->GetTcpSocket()->getHandle(), false)) != 0)
        {
            printf("G_EpollHandler()->SetPeer(peer->get_socket(%d)) %d(%s)",
                   peer->GetTcpSocket()->getHandle(), result, strerror(result));
        }
        m_peers.insert(std::make_pair((int)peer->GetTcpSocket()->getHandle(), peer));
        m_peerQ.pop();
    }
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0018 | monitor | 与ORIG差异=DIFF | CTcpNetSystem::SendPacket | 详见 function_reports/monitor/_ZN13CTcpNetSystem10SendPacketEv.md
int CTcpNetSystem::SendPacket()
{
    register int ret;
    register int flag;
    CTcpSendBuffer* buf;
    CTcpSendBuffer* b2;
    CPeer* peer;
    int result;
    {
        CGuard<CMutex> guard(&m_mutexe8);
        if (m_sendQ.empty())
        {
            ret = 0;
            flag = 0;
        }
        else
        {
            buf = m_sendQ.front();
            flag = 1;
        }
    }
    if (flag)
    {
        if (!buf)
        {
            return 0;
        }
        b2 = buf;
        std::map<unsigned int, CPeer*>::iterator it =
            m_peers.find(((PacketHeader*)b2)->m_connNo);
        if (it == m_peers.end())
        {
            DNF_LOG_SCOPE_LINE(0xba,"./log/TcpSend", "SEND ERR:no peer(id:%d,size:%d,ip:%d)",
                (unsigned int)((PacketHeader*)b2)->packetId,
                (unsigned int)((PacketHeader*)b2)->packetSize,
                ((PacketHeader*)b2)->m_connNo);
            PopDeleteTcpSendPacketQ(buf);
            return 0;
        }
        peer = it->second;
        if (peer == 0 ||
            (int)((PacketHeader*)b2)->m_connNo != peer->GetTcpSocket()->getHandle())
        {
            DNF_LOG_SCOPE_LINE(0xc3,"./log/TcpSend", "SEND ERR:invalid peer(%x)(id:%d)(size:%d)(ip:%d)",
                peer, (unsigned int)((PacketHeader*)b2)->packetId,
                (unsigned int)((PacketHeader*)b2)->packetSize,
                ((PacketHeader*)b2)->m_connNo);
            PopDeleteTcpSendPacketQ(buf);
            return 0;
        }
        result = peer->send_packet((char*)b2, (unsigned int)((PacketHeader*)b2)->packetSize);
        if (result < 1)
        {
            DNF_LOG_SCOPE_LINE(0xd5,"./log/TcpSend", "SEND(id:%d,size:%d,ip:%d, cnt:%d)",
                (unsigned int)((PacketHeader*)b2)->packetId,
                (unsigned int)((PacketHeader*)b2)->packetSize,
                ((PacketHeader*)b2)->m_connNo, (unsigned int)m_sendQ.size());
        }
        else
        {
            PopDeleteTcpSendPacketQ(buf);
        }
        return result;
    }
    return 0;
}

int CTcpNetSystem::WaitForEvent()
{
    return m_handler->WaitForEvent();
}

void CTcpNetSystem::DeletePeer(CPeer* peer)
{
    unsigned int fd = peer->GetTcpSocket()->getHandle();
    std::map<unsigned int, CPeer*>::iterator it = m_peers.find(fd);
    if (it != m_peers.end())
    {
        m_peers.erase(it);
    }
    CGuard<CMutex> guard(&m_mutex78);
    delete peer;
}

CPeer* CTcpNetSystem::CreatePeer()
{
    CGuard<CMutex> guard(&m_mutex78);
    return new CPeer;
}

void CTcpNetSystem::InsertAcceptedPeer(CPeer* peer)
{
    CGuard<CMutex> guard(&m_mutex60);
    m_peerQ.push(peer);
}

CPeer* CTcpNetSystem::GetPeer(unsigned int id)
{
    std::map<unsigned int, CPeer*>::iterator it = m_peers.find(id);
    if (it != m_peers.end())
    {
        return it->second;
    }
    return 0;
}

void CTcpNetSystem::PopDeleteTcpSendPacketQ(CTcpSendBuffer* buf)
{
    {
        CGuard<CMutex> guard1(&m_mutexe8);
        m_sendQ.pop();
    }
    {
        CGuard<CMutex> guard2(&m_mutex100);
        delete buf;
    }
}

// [DNF-NONIDENTICAL] DNF-MON-DIFF-0020 | monitor | 与ORIG差异=DIFF | CTcpNetSystem::CleanTcpSendPacketQ | 详见 function_reports/monitor/_ZN13CTcpNetSystem19CleanTcpSendPacketQEv.md
void CTcpNetSystem::CleanTcpSendPacketQ()
{
    while (true)
    {
        CTcpSendBuffer* buf;
        int flag;
        {
            CGuard<CMutex> guard1(&m_mutexe8);
            if (m_sendQ.empty())
            {
                flag = 0;
            }
            else
            {
                buf = m_sendQ.front();
                m_sendQ.pop();
                flag = 1;
            }
        }
        if (!flag)
        {
            break;
        }
        {
            CGuard<CMutex> guard2(&m_mutex100);
            delete buf;
        }
    }
    DNF_LOG_SCOPE_LINE(0x16b, "./log/TcpSend", "Clean Tcp Send Queue Complete !");
}

void CTcpNetSystem::SetEpollConnectedPeer(CPeer* peer)
{
    {
        CGuard<CMutex> guard(&m_mutex78);
        int rc = 0;
        if ((rc = m_handler->SetPeer(peer, peer->GetTcpSocket()->getHandle(), false)) != 0)
        {
            printf("G_EpollHandler()->SetPeer(peer->get_socket(%d)) %d(%s)",
                   peer->GetTcpSocket()->getHandle(), rc, strerror(rc));
        }
        m_peers.insert(std::make_pair(peer->GetTcpSocket()->getHandle(), peer));
    }
}

unsigned short CTcpNetSystem::Get_TcpServerPort() { return m_port; }

void* CTcpNetSystem::Acquire_TcpSendBuffer()
{
    CGuard<CMutex> guard(&m_mutex100);
    return new CTcpSendBuffer;
}

void CTcpNetSystem::PushTcpSendPacketQ(char* buf)
{
    CGuard<CMutex> guard(&m_mutexe8);
    m_sendQ.push((CTcpSendBuffer*)buf);
    int size = (int)m_sendQ.size();
    if (10 < size)
    {
        DNF_LOG_SCOPE_LINE(0x91,"./log/TcpSend", "SEND PUSH(cnt:%d,id:%d,size:%d,ip:%d)", size,
            (unsigned int)((PacketHeader*)buf)->packetId,
            (unsigned int)((PacketHeader*)buf)->packetSize,
            ((PacketHeader*)buf)->m_connNo);
    }
}

CMutex* CTcpNetSystem::Get_TcpRecvBLock() { return &m_mutexa8; }

CMutex* CTcpNetSystem::Get_TcpRecvQLock() { return &m_mutex90; }

CTcpHandler* CTcpNetSystem::Get_TcpHandler() { return m_handler; }

CMutex* CTcpNetSystem::Get_TcpSendBLock() { return &m_mutex100; }

CMutex* CTcpNetSystem::Get_TcpSendQLock() { return &m_mutexe8; }

std::queue<CTcpSendBuffer*>* CTcpNetSystem::Get_TcpSendQPacket() { return &m_sendQ; }

CSwapQueue<std::queue<CTcpRecvBuffer*, std::deque<CTcpRecvBuffer*, std::allocator<CTcpRecvBuffer*> > >, 2>*
    CTcpNetSystem::Get_TcpSwapQPacket()
{
    return &m_recvSwapQ;
}
